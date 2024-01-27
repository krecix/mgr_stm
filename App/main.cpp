#include "System.hpp"
#include "CCUHardware.hpp"

#include "BinaryCommunicator.hpp"
#include "Parser.hpp"
#include "Pattern.hpp"
#include "ModuleController.hpp"

static ccu::CCUHardware hardware;
static Pattern pattern;

static ModuleController moduleController{
        hardware.VibrationControllerUnit,
        pattern
};

[[noreturn]] void MainTaskProcedure([[maybe_unused]] void *args);

int main() {
    hardware.Initialize();
    System::KernelInit();

    //Go to Initial position
    hardware.VibrationControllerUnit.InitializeProcedure();

    BinaryCommunicator::Instance().Initialize(hardware.Terminal);

    Parser::setModuleController(&moduleController);

    System::Task ccuThread("CCU_Task", 1024, System::RTOSPriority::Normal);
    ccuThread.createTask(MainTaskProcedure, nullptr);

    System::KernelStart();
    while (true);
}

[[noreturn]] void MainTaskProcedure([[maybe_unused]] void *args) {

    while (true) {

        hardware.Leds.greenLed.Toggle();

        osDelay(System::TicksFromMs(100));
    }
}

