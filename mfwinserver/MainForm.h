#pragma once

#include "../system/MicroFurtureSystem.h"
#include "InteroLogSink.h"

namespace Micro
{
	namespace Future{

		using namespace System;
		using namespace System::ComponentModel;
		using namespace System::Collections;
		using namespace System::Windows::Forms;
		using namespace System::Data;
		using namespace System::Drawing;
		using namespace Resources;

		/// <summary>
		/// MainForm 摘要
		/// </summary>
		public ref class MainForm : public System::Windows::Forms::Form
		{
		public:
			MainForm(void)
			{
				InitializeComponent();

				_msgrcv = gcnew DelegateMessageRecv(this, &MainForm::AppendText);

				_logSink = new InteroLogSink();

				_logSink->AddListener(gcnew DelegateMessageRecv(this, &MainForm::OnMessageRecv));

				auto assembly = this->GetType()->Assembly;

				_resouceMgr = gcnew ResourceManager(assembly->GetName()->Name + ".Resource", assembly);

				_RLightIcon = gcnew System::Drawing::Icon(safe_cast<System::Drawing::Icon^>
					(_resouceMgr->GetObject("RLight")), 32, 32);
				_GLightIcon = gcnew System::Drawing::Icon(safe_cast<System::Drawing::Icon^>
					(_resouceMgr->GetObject("GLight")), 32, 32);

				toolStripMenuItemStart->Image = _GLightIcon->ToBitmap();
				toolStripMenuItemStop->Image = _RLightIcon->ToBitmap();
				toolStripMenuItemExit->Image = (gcnew System::Drawing::Icon(safe_cast<System::Drawing::Icon^>
					(_resouceMgr->GetObject("Exit")), 32, 32))->ToBitmap();

				notifyIconStatus->Icon = _RLightIcon;

			}

		protected:
			/// <summary>
			/// 清理所有正在使用的资源。
			/// </summary>
			~MainForm()
			{
				if (components)
				{
					delete components;
				}

				if (_logSink)
					delete _logSink;
			}
		private: System::Windows::Forms::NotifyIcon^  notifyIconStatus;
		private: System::Windows::Forms::RichTextBox^  richTextBoxLog;
		private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip;
		private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemStart;
		private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemStop;
		private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemExit;
		protected:
		private: System::ComponentModel::IContainer^  components;

		public: static const char* CONFIG_FILE = "system";
				/// <summary>
				/// 必需的设计器变量。
				/// </summary>
		private: ResourceManager^ _resouceMgr;
		private: System::Drawing::Icon^ _RLightIcon;
		private: System::Drawing::Icon^ _GLightIcon;;
		private: bool _exiting;
		private: System::Windows::Forms::Timer^  timer;

		private: InteroLogSink* _logSink;
		private: DelegateMessageRecv^ _msgrcv;

#pragma region Windows Form Designer generated code
				 /// <summary>
				 /// 设计器支持所需的方法 - 不要
				 /// 使用代码编辑器修改此方法的内容。
				 /// </summary>
				 void InitializeComponent(void)
				 {
					 this->components = (gcnew System::ComponentModel::Container());
					 this->notifyIconStatus = (gcnew System::Windows::Forms::NotifyIcon(this->components));
					 this->contextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
					 this->toolStripMenuItemStart = (gcnew System::Windows::Forms::ToolStripMenuItem());
					 this->toolStripMenuItemStop = (gcnew System::Windows::Forms::ToolStripMenuItem());
					 this->toolStripMenuItemExit = (gcnew System::Windows::Forms::ToolStripMenuItem());
					 this->richTextBoxLog = (gcnew System::Windows::Forms::RichTextBox());
					 this->timer = (gcnew System::Windows::Forms::Timer(this->components));
					 this->contextMenuStrip->SuspendLayout();
					 this->SuspendLayout();
					 // 
					 // notifyIconStatus
					 // 
					 this->notifyIconStatus->BalloonTipIcon = System::Windows::Forms::ToolTipIcon::Info;
					 this->notifyIconStatus->BalloonTipTitle = L"Prompt";
					 this->notifyIconStatus->ContextMenuStrip = this->contextMenuStrip;
					 this->notifyIconStatus->Text = L"Micro Future Daemon";
					 this->notifyIconStatus->Visible = true;
					 this->notifyIconStatus->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::notifyIconStatus_MouseDoubleClick);
					 // 
					 // contextMenuStrip
					 // 
					 this->contextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
						 this->toolStripMenuItemStart,
							 this->toolStripMenuItemStop, this->toolStripMenuItemExit
					 });
					 this->contextMenuStrip->Name = L"contextMenuStrip";
					 this->contextMenuStrip->Size = System::Drawing::Size(104, 70);
					 // 
					 // toolStripMenuItemStart
					 // 
					 this->toolStripMenuItemStart->Name = L"toolStripMenuItemStart";
					 this->toolStripMenuItemStart->Size = System::Drawing::Size(103, 22);
					 this->toolStripMenuItemStart->Text = L"Start";
					 this->toolStripMenuItemStart->Click += gcnew System::EventHandler(this, &MainForm::toolStripMenuItemStart_Click);
					 // 
					 // toolStripMenuItemStop
					 // 
					 this->toolStripMenuItemStop->Name = L"toolStripMenuItemStop";
					 this->toolStripMenuItemStop->Size = System::Drawing::Size(103, 22);
					 this->toolStripMenuItemStop->Text = L"Stop";
					 this->toolStripMenuItemStop->Click += gcnew System::EventHandler(this, &MainForm::toolStripMenuItemStop_Click);
					 // 
					 // toolStripMenuItemExit
					 // 
					 this->toolStripMenuItemExit->Name = L"toolStripMenuItemExit";
					 this->toolStripMenuItemExit->Size = System::Drawing::Size(103, 22);
					 this->toolStripMenuItemExit->Text = L"Exit";
					 this->toolStripMenuItemExit->Click += gcnew System::EventHandler(this, &MainForm::toolStripMenuItemExit_Click);
					 // 
					 // richTextBoxLog
					 // 
					 this->richTextBoxLog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
						 | System::Windows::Forms::AnchorStyles::Left)
						 | System::Windows::Forms::AnchorStyles::Right));
					 this->richTextBoxLog->BackColor = System::Drawing::Color::Black;
					 this->richTextBoxLog->ContextMenuStrip = this->contextMenuStrip;
					 this->richTextBoxLog->ForeColor = System::Drawing::Color::Lime;
					 this->richTextBoxLog->Location = System::Drawing::Point(0, 0);
					 this->richTextBoxLog->Name = L"richTextBoxLog";
					 this->richTextBoxLog->Size = System::Drawing::Size(486, 462);
					 this->richTextBoxLog->TabIndex = 0;
					 this->richTextBoxLog->Text = L"";
					 // 
					 // timer
					 // 
					 this->timer->Interval = 5000;
					 this->timer->Tick += gcnew System::EventHandler(this, &MainForm::timer_Tick);
					 // 
					 // MainForm
					 // 
					 this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
					 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					 this->ClientSize = System::Drawing::Size(484, 462);
					 this->Controls->Add(this->richTextBoxLog);
					 this->Name = L"MainForm";
					 this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
					 this->Text = L"Micro Future Server Monitor";
					 this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
					 this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
					 this->contextMenuStrip->ResumeLayout(false);
					 this->ResumeLayout(false);

				 }
#pragma endregion
		private: System::Void OnMessageRecv(String^ message)
		{
			richTextBoxLog->BeginInvoke(_msgrcv, message);
		}

		private: void AppendText(String^ message)
		{
			richTextBoxLog->AppendText(message);
			richTextBoxLog->AppendText("\n");
		}

		private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			e->Cancel = !_exiting;

			this->Visible = false;
			notifyIconStatus->BalloonTipText = L"Log window is hidden.\nDouble click to show it.";
			notifyIconStatus->ShowBalloonTip(10000);
		}

		private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e) {

			if (MicroFurtureSystem::Instance()->Load(CONFIG_FILE) &&
				MicroFurtureSystem::Instance()->Run())
			{
				toolStripMenuItemStart->Enabled = false;
				notifyIconStatus->Icon = _GLightIcon;

				timer->Start();
			}

		}

		private: System::Void notifyIconStatus_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			this->Visible = true;
			if (WindowState == FormWindowState::Minimized)
				WindowState = FormWindowState::Normal;
			BringToFront();
		}

		private: System::Void toolStripMenuItemStart_Click(System::Object^  sender, System::EventArgs^  e) {
			MicroFurtureSystem::Instance()->Run();
			toolStripMenuItemStart->Enabled = false;
			toolStripMenuItemStop->Enabled = true;
			notifyIconStatus->Icon = _GLightIcon;
		}
		private: System::Void toolStripMenuItemStop_Click(System::Object^  sender, System::EventArgs^  e) {
			MicroFurtureSystem::Instance()->Stop();
			toolStripMenuItemStart->Enabled = true;
			toolStripMenuItemStop->Enabled = false;
			notifyIconStatus->Icon = _RLightIcon;
		}

		private: System::Void toolStripMenuItemExit_Click(System::Object^  sender, System::EventArgs^  e) {

			_exiting = true;

			if (MicroFurtureSystem::Instance()->IsRunning())
			{
				MicroFurtureSystem::Instance()->Stop();
			}

			google::FlushLogFiles(google::GLOG_INFO);

			notifyIconStatus->Visible = false;

			Application::Exit();
		}

		private: System::Void timer_Tick(System::Object^  sender, System::EventArgs^  e) {
			timer->Stop();
			this->Close();
		}
		};
	}
}
