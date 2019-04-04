#pragma once

namespace CLRApplication {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace System::Threading::Tasks;

	/// <summary>
	/// MyForm 摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  在此处添加构造函数代码
			//

			this->Load += gcnew System::EventHandler(this, &CLRApplication::MyForm::Form_OnLoad);
		}

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;
		Label^ label;
		ProgressBar^ progress;
		static const int N = 20;

		System::Void Form_OnLoad(System::Object^ sender, System::EventArgs^ e)
		{
			InitializeApp();
		}
		System::Void InitializeApp()
		{
			label = gcnew Label();
			label->Location = Point(2, 25);

			progress = gcnew ProgressBar();
			progress->Location = Point(2, 235);
			progress->Width = 280;
			progress->Height = 25;
			progress->Step = 1;
			progress->Maximum = MyForm::N;
			progress->Value = 0;

			this->Controls->Add(label);
			this->Controls->Add(progress);

			//后台线程（关闭程序后，后台线程都将自动终止）
			Task^ task = gcnew Task(gcnew Action<Object^>(&MyForm::StartAddress), this);
			task->Start();
			//后台线程
			//Thread^ thread = gcnew Thread(gcnew ParameterizedThreadStart(&MyForm::StartAddress));
			//thread->IsBackground = true;
			//thread->Start(this);

			//前台线程（只要前台线程还在运行，应用程序就不会退出）
			//Thread^ thread = gcnew Thread(gcnew ParameterizedThreadStart(&MyForm::StartAddress));
			//thread->Start(this);
		}

		static System::Void StartAddress(Object^ parameter)
		{
			MyForm^ form = (MyForm^)parameter;

			UpdateProgressBarDelegate^ action = gcnew UpdateProgressBarDelegate(form, &MyForm::UpdateProgressBar);
			__int64 iResult = 1;

			for (int iIndex = 1; iIndex <= MyForm::N; iIndex++)
			{
				iResult *= iIndex;
				form->BeginInvoke(action);

				System::Threading::Thread::Sleep(10);
			}

			UpdateLabelTextDelegate^ textAction = gcnew UpdateLabelTextDelegate(form, &MyForm::UpdateLabelText);
			form->BeginInvoke(textAction, L"Result:" + Convert::ToString(iResult));
		}

		delegate void UpdateProgressBarDelegate(void);
		System::Void UpdateProgressBar(void)
		{
			progress->PerformStep();
		}

		delegate void UpdateLabelTextDelegate(String^ szText);
		System::Void UpdateLabelText(String^ szText)
		{
			label->AutoSize = false;
			label->Text = szText;
			label->AutoSize = true;
		}

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 262);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	};

	ref class Lock
	{
		Object^ lockObject;
	public:
		Lock(Object^ lock) : lockObject(lock)
		{
			Monitor::Enter(lockObject);
		}
	protected:
		~Lock()
		{
			Monitor::Exit(lockObject);
		}
	};

}
