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
	/// MyForm ժҪ
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  �ڴ˴���ӹ��캯������
			//

			this->Load += gcnew System::EventHandler(this, &CLRApplication::MyForm::Form_OnLoad);
		}

	protected:
		/// <summary>
		/// ������������ʹ�õ���Դ��
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
		/// ����������������
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

			//��̨�̣߳��رճ���󣬺�̨�̶߳����Զ���ֹ��
			Task^ task = gcnew Task(gcnew Action<Object^>(&MyForm::StartAddress), this);
			task->Start();
			//��̨�߳�
			//Thread^ thread = gcnew Thread(gcnew ParameterizedThreadStart(&MyForm::StartAddress));
			//thread->IsBackground = true;
			//thread->Start(this);

			//ǰ̨�̣߳�ֻҪǰ̨�̻߳������У�Ӧ�ó���Ͳ����˳���
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
		/// �����֧������ķ��� - ��Ҫ
		/// ʹ�ô���༭���޸Ĵ˷��������ݡ�
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
