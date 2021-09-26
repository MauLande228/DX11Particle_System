#include "KeyBoard.h"

bool KeyBoard::KeyIsPressed(unsigned char code) const noexcept
{
    return keyStates[code];
}

std::optional<KeyBoard::Event> KeyBoard::ReadKey() noexcept
{
    if (keyBuffer.size() > 0)
    {
        KeyBoard::Event e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    return {};
}

bool KeyBoard::KeyIsEmpty() const noexcept
{
    return keyBuffer.empty();
}

void KeyBoard::FlushKey() noexcept
{
    keyBuffer = std::queue<Event>();
}

std::optional<char> KeyBoard::ReadChar() noexcept
{
    if (charBuffer.size() > 0)
    {
        unsigned char code = charBuffer.front();
        charBuffer.pop();
        return code;
    }
    return {};
}

bool KeyBoard::CharIsEmpty() const noexcept
{
    return charBuffer.empty();
}

void KeyBoard::FlushChar() noexcept
{
    charBuffer = std::queue<char>();
}

void KeyBoard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

void KeyBoard::EnableAutoRepeat() noexcept
{
    autoRepeatEnabled = true;
}

void KeyBoard::DisableAutorepeat() noexcept
{
    autoRepeatEnabled = false;
}

bool KeyBoard::IsAutoRepeatEnabled() const noexcept
{
    return autoRepeatEnabled;
}

void KeyBoard::OnKeyPressed(unsigned char code) noexcept
{
    keyStates[code] = true;
    keyBuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Press, code));
    TrimBuffer(keyBuffer);
}

void KeyBoard::OnKeyReleased(unsigned char code) noexcept
{
    keyStates[code] = false;
    keyBuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Release, code));
    TrimBuffer(keyBuffer);
}

void KeyBoard::OnChar(char character) noexcept
{
    charBuffer.push(character);
    TrimBuffer(charBuffer);
}

void KeyBoard::ClearState() noexcept
{
    keyStates.reset();
}

template<typename T>
void KeyBoard::TrimBuffer(std::queue<T>& buffer) noexcept
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}
