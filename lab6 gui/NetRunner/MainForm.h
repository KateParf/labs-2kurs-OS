#pragma once

namespace NetRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	/// <summary>
	/// форма MyForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Panel^ panelButtons;

	private: System::Windows::Forms::Button^ buttonStart;
	private: System::Windows::Forms::Panel^ panelLog;
	
	private: System::Windows::Forms::TextBox^ textBoxLog;

	private: System::Windows::Forms::Button^ buttonSave;

	private: System::Windows::Forms::Button^ buttonLoad;
	private: System::Windows::Forms::Label^ labelCount;

	private: System::Windows::Forms::NumericUpDown^ numCount;
	private: System::Windows::Forms::Panel^ panelNumCount;
	private: System::Windows::Forms::OpenFileDialog^ openFileDialog1;
	private: System::Windows::Forms::SaveFileDialog^ saveFileDialog1;

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->panelButtons = (gcnew System::Windows::Forms::Panel());
			this->panelNumCount = (gcnew System::Windows::Forms::Panel());
			this->numCount = (gcnew System::Windows::Forms::NumericUpDown());
			this->labelCount = (gcnew System::Windows::Forms::Label());
			this->buttonSave = (gcnew System::Windows::Forms::Button());
			this->buttonLoad = (gcnew System::Windows::Forms::Button());
			this->buttonStart = (gcnew System::Windows::Forms::Button());
			this->panelLog = (gcnew System::Windows::Forms::Panel());
			this->textBoxLog = (gcnew System::Windows::Forms::TextBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->panelButtons->SuspendLayout();
			this->panelNumCount->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numCount))->BeginInit();
			this->panelLog->SuspendLayout();
			this->SuspendLayout();
			// 
			// panelButtons
			// 
			this->panelButtons->Controls->Add(this->panelNumCount);
			this->panelButtons->Controls->Add(this->labelCount);
			this->panelButtons->Controls->Add(this->buttonSave);
			this->panelButtons->Controls->Add(this->buttonLoad);
			this->panelButtons->Controls->Add(this->buttonStart);
			this->panelButtons->Dock = System::Windows::Forms::DockStyle::Top;
			this->panelButtons->Location = System::Drawing::Point(0, 0);
			this->panelButtons->Name = L"panelButtons";
			this->panelButtons->Size = System::Drawing::Size(933, 73);
			this->panelButtons->TabIndex = 0;
			// 
			// panelNumCount
			// 
			this->panelNumCount->BackColor = System::Drawing::Color::White;
			this->panelNumCount->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panelNumCount->Controls->Add(this->numCount);
			this->panelNumCount->Location = System::Drawing::Point(212, 13);
			this->panelNumCount->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->panelNumCount->Name = L"panelNumCount";
			this->panelNumCount->Size = System::Drawing::Size(99, 48);
			this->panelNumCount->TabIndex = 1;
			// 
			// numCount
			// 
			this->numCount->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->numCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->numCount->Location = System::Drawing::Point(5, 7);
			this->numCount->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numCount->Name = L"numCount";
			this->numCount->Size = System::Drawing::Size(87, 30);
			this->numCount->TabIndex = 1;
			this->numCount->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			// 
			// labelCount
			// 
			this->labelCount->AutoSize = true;
			this->labelCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelCount->Location = System::Drawing::Point(12, 27);
			this->labelCount->Name = L"labelCount";
			this->labelCount->Size = System::Drawing::Size(168, 25);
			this->labelCount->TabIndex = 4;
			this->labelCount->Text = L"Кол-во клиентов";
			// 
			// buttonSave
			// 
			this->buttonSave->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->buttonSave->BackColor = System::Drawing::SystemColors::ControlLight;
			this->buttonSave->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->buttonSave->FlatAppearance->BorderColor = System::Drawing::Color::DarkOrange;
			this->buttonSave->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->buttonSave->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"buttonSave.Image")));
			this->buttonSave->Location = System::Drawing::Point(849, 13);
			this->buttonSave->Name = L"buttonSave";
			this->buttonSave->Size = System::Drawing::Size(68, 49);
			this->buttonSave->TabIndex = 3;
			this->buttonSave->UseVisualStyleBackColor = false;
			this->buttonSave->Click += gcnew System::EventHandler(this, &MainForm::buttonSave_Click);
			// 
			// buttonLoad
			// 
			this->buttonLoad->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->buttonLoad->BackColor = System::Drawing::SystemColors::ControlLight;
			this->buttonLoad->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->buttonLoad->FlatAppearance->BorderColor = System::Drawing::Color::DarkOrange;
			this->buttonLoad->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->buttonLoad->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"buttonLoad.Image")));
			this->buttonLoad->Location = System::Drawing::Point(775, 13);
			this->buttonLoad->Name = L"buttonLoad";
			this->buttonLoad->Size = System::Drawing::Size(65, 49);
			this->buttonLoad->TabIndex = 2;
			this->buttonLoad->UseVisualStyleBackColor = false;
			this->buttonLoad->Click += gcnew System::EventHandler(this, &MainForm::buttonLoad_Click);
			// 
			// buttonStart
			// 
			this->buttonStart->BackColor = System::Drawing::SystemColors::ControlLight;
			this->buttonStart->FlatAppearance->BorderColor = System::Drawing::Color::DarkOrange;
			this->buttonStart->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->buttonStart->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->buttonStart->Location = System::Drawing::Point(324, 13);
			this->buttonStart->Name = L"buttonStart";
			this->buttonStart->Size = System::Drawing::Size(156, 49);
			this->buttonStart->TabIndex = 1;
			this->buttonStart->Text = L"Старт";
			this->buttonStart->UseVisualStyleBackColor = false;
			this->buttonStart->Click += gcnew System::EventHandler(this, &MainForm::buttonStart_Click);
			// 
			// panelLog
			// 
			this->panelLog->Controls->Add(this->textBoxLog);
			this->panelLog->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelLog->Location = System::Drawing::Point(0, 73);
			this->panelLog->Name = L"panelLog";
			this->panelLog->Padding = System::Windows::Forms::Padding(7, 6, 7, 6);
			this->panelLog->Size = System::Drawing::Size(933, 400);
			this->panelLog->TabIndex = 2;
			// 
			// textBoxLog
			// 
			this->textBoxLog->BackColor = System::Drawing::Color::White;
			this->textBoxLog->Dock = System::Windows::Forms::DockStyle::Fill;
			this->textBoxLog->Location = System::Drawing::Point(7, 6);
			this->textBoxLog->Multiline = true;
			this->textBoxLog->Name = L"textBoxLog";
			this->textBoxLog->ReadOnly = true;
			this->textBoxLog->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBoxLog->Size = System::Drawing::Size(919, 388);
			this->textBoxLog->TabIndex = 0;
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->Filter = L"Log files|*.log|Text files|*.txt";
			this->openFileDialog1->InitialDirectory = L"c:\\";
			// 
			// saveFileDialog1
			// 
			this->saveFileDialog1->Filter = L"Log files|*.log|Text files|*.txt";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(933, 473);
			this->Controls->Add(this->panelLog);
			this->Controls->Add(this->panelButtons);
			this->MinimumSize = System::Drawing::Size(670, 349);
			this->Name = L"MainForm";
			this->Text = L"Робин и пиво";
			this->panelButtons->ResumeLayout(false);
			this->panelButtons->PerformLayout();
			this->panelNumCount->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numCount))->EndInit();
			this->panelLog->ResumeLayout(false);
			this->panelLog->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	// загрузка лога из файла
	private: System::Void buttonLoad_Click(System::Object^ sender, System::EventArgs^ e) {
		// open load dialog
		if (this->openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			String^ strfilename = this->openFileDialog1->FileName;
			String^ Readfile = File::ReadAllText(strfilename);

			textBoxLog->Text = "";
			textBoxLog->AppendText(Readfile);
		}
	}

	// запись лога в файл
	private: System::Void buttonSave_Click(System::Object^ sender, System::EventArgs^ e) {
		// open save dialog
		if (this->saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			String^ strfilename = this->saveFileDialog1->FileName;
			File::WriteAllText(strfilename, textBoxLog->Text);
		}
	}

	// обработчик нажатия на кнопку Старт процессов
	private: System::Void buttonStart_Click(System::Object^ sender, System::EventArgs^ e) {
		panelButtons->Enabled = false;
		textBoxLog->Text = "";
		int cnt = int(numCount->Value);
		onStart(gcnew appendLogHandler(this, &MainForm::appendLog), cnt);
	}


	public: 
		delegate Void appendLogHandler(String^ text);

		Void appendLog(String^ text) {
			if (this->textBoxLog->InvokeRequired) {
				this->textBoxLog->Invoke(gcnew appendLogHandler(this, &MainForm::doAppendLog), text);
			}
			else {
				doAppendLog(text);
			}
		}

		Void doAppendLog(String^ text) {
			if (text == "\z") {
				panelButtons->Enabled = true;
			} else
				textBoxLog->AppendText(text + "\r\n");
		}

		delegate Void onStartHandler(appendLogHandler^ appendLogFunc, int procCount);

		// внешний обработчик нажатия на кнопку Старт
		onStartHandler^ onStart;
};
}
