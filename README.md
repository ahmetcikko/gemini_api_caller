# Pardus Jarvis

[Türkçe](#türkçe) | [English](#english)

## Türkçe

Pardus için sesle çalışan yapay zeka asistanı. Arka planda çalışan daemon "Hey Jarvis" dediğinizi duyunca ana programı açar. Açmak istediğiniz programı belirttiğinizde onu sizin için açar.

### Nasıl çalışır

1. `Pardus-Jarvis-Daemon` açılışta arka planda çalışır ve mikrofonu yerel olarak dinler (Lowwi wakeword modeli, internete veri gitmez).
2. "Hey Jarvis" algılanınca asistan arayüzü açılır ve kayıt başlar.
3. Konuşma bittiğinde (1.3 saniyelik sessizlikten veya en fazla 10 saniyelik kayıttan sonra) ses WebRTC ile gürültüden arındırılır ve whisper.cpp ile yerel olarak yazıya çevrilir. Vulkan destekli GPU varsa otomatik kullanılır, yoksa CPU'da çalışır.
4. Yazıya çevrilen komut Groq API üzerinden llama-3.1-8b-instant modeline gönderilir. Model komutu JSON aksiyona çevirir: uygulama aç, uygulama kapat, site aç, ses seviyesini ayarla, bilgisayarı kapat/yeniden başlat/uyku moduna al veya sohbet cevabı. Uygulama listeleri her istekte değil, yalnızca gerektiğinde (açma için kurulu uygulamalar, kapatma için çalışan işlemler) ikinci bir kısa istekle gönderilir. Sistem kapatma/yeniden başlatma/uyku yalnızca "bilgisayarı kapat" gibi açıkça belirtilmiş komutlarda tetiklenir; tek başına "kapat" uygulama kapatma olarak yorumlanır. Kritik sistem süreçleri (gnome-shell, Xorg, systemd vb.) hiçbir zaman kapatma hedefi olarak sunulmaz.
5. Uygulama açma istekleri sistemdeki .desktop girdileri üzerinden çözülür (XDG standardı, Flatpak dahil), siteler xdg-open ile açılır.

Sesli komutlar her zaman Türkçe algılanır. Uygulama menüde ve aramada görünen tek program `Pardus Jarvis` (Ayarlar) uygulamasıdır; daemon ve asistan penceresi arka planda kalır, menüde görünmez.

### Proje yapısı

- `app/` — asistan penceresi (kayıt, transkripsiyon, Groq isteği, aksiyon çalıştırma)
- `daemon/` — arka planda çalışan wakeword dinleyici
- `settings/` — mikrofon, dil ve API anahtarı ayarları, kaldırma
- `images/`, `sounds/` — paylaşılan ikon ve ses dosyaları

### Derleme

Gerekli paketler:

```
sudo apt install cmake ninja-build git clang pkg-config qt6-base-dev qt6-declarative-dev qt6-tools-dev libwebrtc-audio-processing-dev
```

(`qt6-tools-dev` çeviri dosyalarını derlemek için gerekli — `lrelease` bu pakette gelir.)

GPU hızlandırma için (isteğe bağlı):

```
sudo apt install glslc libvulkan-dev spirv-headers
```

Derleme:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

İlk yapılandırmada whisper modeli (~190MB, `ggml-small-q5_1.bin`) ve bağımlılıklar otomatik indirilir.

Kaynaktan derlerken varsayılan Groq API anahtarını `app/apikey.h` dosyasındaki `kDefaultApiKey` sabitine yazabilirsiniz (bu dosya .gitignore'dadır, deponuza asla işlenmez). Paketlenmiş sürümde bu adıma gerek yoktur — bkz. aşağıdaki Ayarlar bölümü. Ücretsiz anahtar: https://console.groq.com

### .deb paketi oluşturma

```
cd build
cpack
sudo dpkg -i pardusjarvis_1.4.2_amd64.deb
sudo apt install -f
```

Kurulum sonrası daemon ve Ayarlar uygulaması otomatik başlar; daemon her açılışta kendiliğinden yeniden başlar.

### Çalıştırma (kaynak derlemesinden)

```
./build/Pardus-Jarvis-Daemon
```

Daemon arka plana geçer ve "Hey Jarvis" demenizi bekler. Asistanı tek seferlik test etmek için doğrudan da açabilirsiniz:

```
./build/"Pardus Jarvis"
```

Ayarlar uygulamasını açmak için:

```
./build/Pardus-Jarvis-Settings
```

### Ayarlar

Uygulama menüsünden "Pardus Jarvis" aratarak açılabilir. Üç ana seçenek sırasıyla:

- **Mikrofon** — kayıt cihazını seçin, değişiklik daemona anında bildirilir (yeniden başlatma gerekmez).
- **Dil** — arayüz dili (Türkçe/İngilizce). Sesli komutlar bundan etkilenmez, her zaman Türkçe algılanır.
- **Gelişmiş** — kendi Groq API anahtarınızı girebilir veya varsayılana sıfırlayabilirsiniz (yalnızca Groq anahtarları desteklenir), ve uygulamayı tamamen kaldırabilirsiniz. Her ikisi de kazara dokunmayı önlemek için iki adımlı onay gerektirir.

### Lisans

GPL-3.0

## English

The project is a voice-activated AI assistant for Pardus. You say "Hey Jarvis" and it boots the listener, then you can tell it what you want it to boot and it'll do it for you.

### How it works

1. `Pardus-Jarvis-Daemon` runs in the background at startup and listens to the microphone locally (Lowwi wakeword model, no data leaves your machine).
2. When "Hey Jarvis" is detected, the assistant UI opens and recording starts.
3. When you stop speaking (1.3 seconds of silence, or a 10 second hard cap) the audio is denoised with WebRTC and transcribed locally with whisper.cpp. A Vulkan-capable GPU is used automatically if present, otherwise it falls back to CPU.
4. The transcribed command is sent to the llama-3.1-8b-instant model via the Groq API. The model maps the command to a JSON action: open an app, close an app, open a URL, adjust the volume, reboot/shut down/sleep the computer, or a chat reply. App lists are not sent with every request — only when needed (installed apps for opening, running processes for closing) via a short second request. System shutdown/reboot/sleep only fires on explicit phrasing like "shut down the computer" — a bare "close" is always treated as closing an app. Critical system processes (gnome-shell, Xorg, systemd, etc.) are never offered as a close target.
5. App launch requests are resolved against the system's .desktop entries (XDG standard, Flatpak included); websites are opened with xdg-open.

Voice commands are always recognized in Turkish. The only app visible in menus/search is `Pardus Jarvis` (Settings) — the daemon and the assistant window stay hidden from the menu by design.

### Project layout

- `app/` — the assistant window (recording, transcription, Groq request, action dispatch)
- `daemon/` — the background wakeword listener
- `settings/` — microphone, language and API key settings, uninstall
- `images/`, `sounds/` — shared icon and sound assets

### Building

Required packages:

```
sudo apt install cmake ninja-build git clang pkg-config qt6-base-dev qt6-declarative-dev qt6-tools-dev libwebrtc-audio-processing-dev
```

(`qt6-tools-dev` is needed to build the translation files — it provides `lrelease`.)

For GPU acceleration (optional):

```
sudo apt install glslc libvulkan-dev spirv-headers
```

Build:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The first configure automatically downloads the whisper model (~190MB, `ggml-small-q5_1.bin`) and all dependencies.

When building from source, you can set the default Groq API key in the `kDefaultApiKey` constant in `app/apikey.h` (this file is gitignored and never committed). Packaged installs don't need this — see Settings below. Free keys: https://console.groq.com

### Building the .deb

```
cd build
cpack
sudo dpkg -i pardusjarvis_1.4.2_amd64.deb
sudo apt install -f
```

After install, the daemon and Settings app both launch automatically; the daemon also relaunches on every subsequent login.

### Running (from a source build)

```
./build/Pardus-Jarvis-Daemon
```

The daemon goes to the background and waits for you to say "Hey Jarvis". You can also launch the assistant directly for a one-off test:

```
./build/"Pardus Jarvis"
```

To open the settings app:

```
./build/Pardus-Jarvis-Settings
```

### Settings

Reachable from the app menu by searching "Pardus Jarvis". Three main options, in order:

- **Microphone** — pick the capture device; the daemon picks up the change instantly (no restart needed).
- **Language** — interface language (Turkish/English). This doesn't affect voice recognition, which always stays Turkish.
- **Advanced** — set your own Groq API key or reset to the default (only Groq keys are supported), and fully uninstall the app. Both require a two-step confirmation to prevent accidental taps.

### License

GPL-3.0
