#include "MainForm.h"

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace NetRunner;

//-----

public ref class Logic {
public:
    Logic(MainForm::appendLogHandler^ printLog, int procCount) {
        this->printLog = printLog;
        this->procCount = procCount;
    }

private:
    MainForm::appendLogHandler^ printLog;
    int procCount;
    int processesStarted = 0;
    Process^ server;

    // ������� �� ������� ��������� ��������
    Void Process_Exited(Object^ sender, System::EventArgs^ e) {
        processesStarted--;
        if (processesStarted == 1) { // ������ ��� �� ������� - ���� ��������� �������
            this->FinishAll();
        }
    }

    // ������������� ������
    Void FinishAll() {
        System::Threading::Thread::Sleep(10000);
        server->Kill();

        printLog("[R U N N E R]  � � � � �   � � � � � � � � �");
        printLog("\z");
    }

    // ���������� ������ �������� � �������
    Void ProcessDataReceived(Object^ sendingProcess, DataReceivedEventArgs^ outLine) {
        if (outLine->Data) {
            String^ str = outLine->Data->ToString();
            printLog(str);
        }
    }

    Process^ RunProcess(String^ exeName, String^ param) {
        Process^ prc = gcnew Process();
        //inialize 
        prc->StartInfo->FileName = exeName;
        prc->StartInfo->Arguments = param;
        prc->StartInfo->UseShellExecute = false;
        prc->StartInfo->CreateNoWindow = true;
        prc->StartInfo->RedirectStandardOutput = true;

        prc->EnableRaisingEvents = true;
        prc->Exited += gcnew EventHandler(this, &Logic::Process_Exited);

        if (prc->Start() == true) {
            processesStarted++;
            prc->OutputDataReceived += gcnew DataReceivedEventHandler(this, &Logic::ProcessDataReceived);
            prc->BeginOutputReadLine();
        }

        return prc;
    }

public:
    Void StartFunction() {
        printLog("[R U N N E R]  � � � � �   � � � � � � � � �");

        // ������ �������
        server = RunProcess("server.exe", "");
        System::Threading::Thread::Sleep(5000);

        // ������ ��������
        for (int i = 0; i < procCount; i++) 
            RunProcess("client.exe", (i + 1).ToString());
    }

};

//-----

// ���������� ������� �� ������ �����
Void onStart(MainForm::appendLogHandler^ printLog, int procCount) {
    Logic^ logic = gcnew Logic(printLog, procCount);

    Thread^ myThread = gcnew Thread(gcnew ThreadStart(logic, &Logic::StartFunction));
    myThread->Start();
}


[STAThreadAttribute]
void Main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    MainForm form;

    form.onStart = gcnew NetRunner::MainForm::onStartHandler(onStart);
    Application::Run(% form);
}

