#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <atomic>
#include <fcntl.h>
#include <filesystem>
#include <linux/limits.h>
#include <lowwi.hpp>
#include <sys/file.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

static std::atomic_bool g_running = false;

void wakeword_callback(CLFML::LOWWI::Lowwi_ctx_t, std::shared_ptr<void>) {

    if (g_running.exchange(true))
        return;
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {const_cast<char *>("Pardus Jarvis"), nullptr};
        char exe[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        exe[len] = '\0';
        std::string backend =
            std::filesystem::path(exe).parent_path().string() +
            "/Pardus Jarvis";
        execv(backend.c_str(), args);
        _exit(-1);
    }
    std::thread([pid]() {
        waitpid(pid, nullptr, 0);
        g_running.store(false);
    }).detach();
}
static void call_back(ma_device *pDevice, void *, const void *pInput,
                      ma_uint32 frameCount) {
    auto *runtime = static_cast<CLFML::LOWWI::Lowwi *>((*pDevice).pUserData);
    const float *samples = static_cast<const float *>(pInput);
    (*runtime).run(std::vector<float>(samples, samples + frameCount));
}
int main() {
    if (fork() > 0)
        _exit(0);
    setsid();
    int devnull = open("/dev/null", O_RDWR);
    dup2(devnull, STDIN_FILENO);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    int pid_fd = open("/tmp/pardusjarvis.pid", O_RDWR | O_CREAT, 0644);
    if (flock(pid_fd, LOCK_EX | LOCK_NB) == -1) {
        close(pid_fd);
        return 0;
    }
    char exe[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    exe[len] = '\0';
    chdir(std::filesystem::path(exe).parent_path().string().c_str());
    CLFML::LOWWI::Lowwi ww_runtime;
    CLFML::LOWWI::Lowwi_word_t ww;
    ww.cbfunc = wakeword_callback;
    ww.threshold = 0.3f;
    ww.min_activations = 1;
    ww.refractory = 20;
    ww.model_path = "models/example_wakewords/hey_jarvis.onnx";
    ww.phrase = "Hey Jarvis";
    ww_runtime.add_wakeword(ww);
    ma_context context;
    ma_context_init(nullptr, 0, nullptr, &context);
    ma_device_info *captureInfos;
    ma_uint32 captureCount;
    ma_context_get_devices(&context, nullptr, nullptr, &captureInfos,
                           &captureCount);
    ma_device device;
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 16000;
    config.dataCallback = call_back;
    config.pUserData = &ww_runtime;
    config.capture.pDeviceID = &captureInfos[2].id;
    ma_device_init(&context, &config, &device);
    ma_device_start(&device);
    pause();
}