#pragma once
#include <queue>
#include <bitset>
#include <optional>

class KeyBoard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press, Release
		};

		Event(Type type, unsigned char code)
			:type(type), code(code)
		{}

		bool IsPressed() const noexcept
		{
			return type == Type::Press;
		}

		bool IsReleased() const noexcept
		{
			return type == Type::Release;
		}

		unsigned char GetCode() const noexcept
		{
			return code;
		}

	private:
		Type type;
		unsigned char code;
	};

public:
	KeyBoard() = default;
	KeyBoard(const KeyBoard&) = delete;
	KeyBoard& operator=(const KeyBoard&) = delete;

	bool KeyIsPressed(unsigned char code) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;

	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void EnableAutoRepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;
	
private:
	void OnKeyPressed(unsigned char code) noexcept;
	void OnKeyReleased(unsigned char code) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autoRepeatEnabled = false;
	std::bitset<nKeys> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};