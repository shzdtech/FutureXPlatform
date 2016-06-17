#include "system_exp.h"
#include "MicroFurtureSystem.h"
#include "MicroFutureLogSink.h"

extern "C" {
	SYSTEM_CLASS_EXPORT void MicroFutureSystem_InitLogger(const char* logPath) {
		MicroFurtureSystem::InitLogger(logPath);
	}

	SYSTEM_CLASS_EXPORT bool MicroFutureSystem_Load(const char* configFile) {
		if (!configFile) configFile = MICROFUTURE_DEFAULT_CONFIG_NAME;
		return MicroFurtureSystem::Instance()->Load(configFile);
	}

	extern "C" SYSTEM_CLASS_EXPORT bool MicroFutureSystem_IsRunning() {
		return MicroFurtureSystem::Instance()->IsRunning();
	}

	SYSTEM_CLASS_EXPORT bool MicroFutureSystem_Start() {
		return MicroFurtureSystem::Instance()->Run();
	}

	SYSTEM_CLASS_EXPORT bool MicroFutureSystem_Stop() {
		return MicroFurtureSystem::Instance()->Stop();
	}

	SYSTEM_CLASS_EXPORT void* CreateLogSink(LogCallbackFn logCallbackFn) {
		return new MicroFutureLogSink(logCallbackFn);
	}

	SYSTEM_CLASS_EXPORT void FreeLogSink(void* logSink) {
		delete logSink;
	}
}