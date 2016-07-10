#include "system_exp.h"
#include "MicroFurtureSystem.h"
#include "../utility/AbstractLogSink.h"

extern "C" {
	MFSYSTEM_CLASS_EXPORT void MicroFutureSystem_InitLogger(const char* logPath) {
		MicroFurtureSystem::InitLogger(logPath);
	}

	MFSYSTEM_CLASS_EXPORT bool MicroFutureSystem_Load(const char* configFile) {
		if (!configFile) configFile = MICROFUTURE_DEFAULT_CONFIG_NAME;
		return MicroFurtureSystem::Instance()->Load(configFile);
	}

	extern "C" MFSYSTEM_CLASS_EXPORT bool MicroFutureSystem_IsRunning() {
		return MicroFurtureSystem::Instance()->IsRunning();
	}

	MFSYSTEM_CLASS_EXPORT bool MicroFutureSystem_Start() {
		return MicroFurtureSystem::Instance()->Start();
	}

	MFSYSTEM_CLASS_EXPORT bool MicroFutureSystem_Stop() {
		return MicroFurtureSystem::Instance()->Stop();
	}

	MFSYSTEM_CLASS_EXPORT void* CreateLogSink(LogCallbackFn logCallbackFn) {
		return AbstractLogSink::CreateSink(logCallbackFn);
	}

	MFSYSTEM_CLASS_EXPORT void FreeLogSink(void* logSink) {
		delete logSink;
	}
}