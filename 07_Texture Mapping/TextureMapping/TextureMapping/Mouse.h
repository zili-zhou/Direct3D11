#include<windows.h>
#include<memory>

namespace DirectX
{
	class Mouse
	{

	public:
		Mouse() noexcept(false);
		Mouse(Mouse&& moveFrom) noexcept;
		Mouse& operator= (Mouse&& moveFrom) noexcept;

		Mouse(Mouse const&) = delete;
		Mouse& operator=(Mouse const&) = delete;

		virtual ~Mouse();

		enum Mode
		{
			MODE_ABSOLUTE = 0,// ��������ģʽ��ÿ��״̬����xyֵΪ��Ļ�������꣬�����ɼ�
			MODE_RELATIVE,  // ����˶�ģʽ��ÿ��״̬����xyֵΪÿһ֮֡�������λ����������겻�ɼ�
		};

		struct State
		{
			bool    leftButton;         // ������������
			bool    middleButton;       // �����ּ�������
			bool    rightButton;        // ����Ҽ�������
			bool    xButton1;           // ����
			bool    xButton2;           // ����
			int     x;                  // ��������x�����ƫ����
			int     y;                  // ��������y�����ƫ����
			int     scrollWheelValue;   // ���ֹ����ۻ�ֵ
			Mode    positionMode;       // ���ģʽ
		};

		class ButtonStateTracker
		{
		public:
			enum ButtonState
			{
				UP = 0,         // ��ťδ������
				HELD = 1,       // ��ť������
				RELEASED = 2,   // ��ť�ձ��ſ�
				PRESSED = 3,    // ��ť�ձ�����
			};

			ButtonState leftButton;     // ������״̬
			ButtonState middleButton;   // �����ְ���״̬
			ButtonState rightButton;    // ����Ҽ�״̬
			ButtonState xButton1;       // ����
			ButtonState xButton2;       // ����

#pragma prefast(suppress: 26495, "Reset() performs the initialization")
			ButtonStateTracker() noexcept { Reset(); }

			void __cdecl Update(const State& state);

			void __cdecl Reset() noexcept;

			State __cdecl GetLastState() const { return lastState; }

		private:
			State lastState;
		};

		

		// Retrieve the current state of the mouse
		State __cdecl GetState() const;

		// Resets the accumulated scroll wheel value
		void __cdecl ResetScrollWheelValue();

		// Sets mouse mode (defaults to absolute)
		void __cdecl SetMode(Mode mode);

		// Feature detection
		bool __cdecl IsConnected() const;

		// Cursor visibility
		bool __cdecl IsVisible() const;
		void __cdecl SetVisible(bool visible);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) && defined(WM_USER)
		void __cdecl SetWindow(HWND window); //���������Ҫ�󶨵Ĵ��ھ��
		static void __cdecl ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
#endif
		// Singleton
		static Mouse& __cdecl Get();

	private:
		// Private implementation.
		class Impl;

		std::unique_ptr<Impl> pImpl;
	};
}