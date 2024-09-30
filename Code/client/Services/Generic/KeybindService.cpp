﻿#include "Services/KeybindService.h"

#include "DInputHook.hpp"

KeybindService::KeybindService(InputService& aInputService) : m_inputService(aInputService)
{
    m_inputHook = &TiltedPhoques::DInputHook::Get();

    m_keyPressConnection = m_inputHook->OnKeyPress.Connect(std::bind(&KeybindService::OnDirectInputKeyPress, this, std::placeholders::_1));

    SetupConfig();
}

void KeybindService::SetupConfig() noexcept
{
    config.ini.SetUnicode(true);

    const auto modPath = TiltedPhoques::GetPath();
    config.path = modPath / Config::kConfigPathName / Config::kKeybindsFileName;

    if (!exists(modPath / Config::kConfigPathName))
    {
        create_directory(modPath / Config::kConfigPathName);
    }

    if (!exists(config.path))
    {
        spdlog::info("{} not found, creating...", Config::kKeybindsFileName);

        if (!config.Create())
        {
            m_uiKey.first = "F2";
            m_debugKey.first = "F3";
        }
    }
    else
    {
        spdlog::info("{} found, loading...", Config::kKeybindsFileName);

        if (config.Load())
        {
            spdlog::info("Successfully loaded {}", Config::kKeybindsFileName);

            m_uiKey.first = config.ini.GetValue("UI", "sUiKey", "F2");
            m_debugKey.first = config.ini.GetValue("UI", "sDebugKey", "F3");
        }
        else
        {
            spdlog::warn("{} Failed to load {}", __FUNCTION__, Config::kKeybindsFileName);
        }
    }
}

bool KeybindService::SetDebugKey(std::shared_ptr<KeybindService::Key> apKey) noexcept
{
    m_debugKey = *apKey;
    config.SetKey("UI", "sDebugKey", m_debugKey.first.c_str());

    return true;
}

void KeybindService::BindNewKey(int32_t aKeyCode) noexcept
{
    m_keyPressConnection = m_inputHook->OnKeyPress.Connect(std::bind(&KeybindService::OnDirectInputKeyPress, this, std::placeholders::_1));

    m_keybindConfirmed = false;
    m_keyCode = aKeyCode;

    TiltedPhoques::String newName = {static_cast<char>(toupper(ConvertToUnicode(aKeyCode)))};

    // Still inserts a null terminator if not found :/
    if (newName.starts_with('\0') && newName.length() == 1)
    {
        auto modKey = std::ranges::find_if(m_modifiedKeys, [&](const std::pair<Key::first_type, int32_t>& acKey) { return acKey.second == m_keyCode; });

        if (modKey != m_modifiedKeys.end())
        {
            newName = modKey->first;
            m_keyCode = modKey->second;
        }
    }

    m_uiKey = {newName, {m_keyCode, 0}};
    m_inputService.SetUIKey(std::make_shared<Key>(m_uiKey));
    config.SetKey("UI", "sUiKey", newName.c_str());
}

void KeybindService::OnDirectInputKeyPress(unsigned long aKeyCode)
{
    if (!m_keybindConfirmed || m_keyCode != 0)
    {
        m_keybindConfirmed = true;

        if (m_keyCode == 0)
        {
            for (int key = 255; key > 1; key--)
            {
                if (GetAsyncKeyState(key) & 0x8000)
                {
                    m_keyCode = key;
                    break;
                }
            }
        }

        if (m_keyCode != 0)
        {
            const TiltedPhoques::String& key = {static_cast<char>(toupper(ConvertToUnicode(m_keyCode)))};
            bool foundKey = false;

            spdlog::info("{} {}", key.c_str(), m_uiKey.first.c_str());

            if (key != m_uiKey.first)
            {
                // If keyname does not match, check if it is a modified key we are looking for
                auto modKey = std::ranges::find_if(m_modifiedKeys, [&](const std::pair<Key::first_type, int32_t>& acKey) { return acKey.second == m_keyCode; });

                if (modKey != m_modifiedKeys.end())
                {
                    m_uiKey.first = modKey->first;
                    m_keyCode = modKey->second;
                    foundKey = true;
                }
            }
            else
            {
                foundKey = true;
            }

            if (foundKey)
            {
                m_uiKey = {m_uiKey.first, {m_keyCode, aKeyCode}};
                m_inputService.SetUIKey(std::make_shared<Key>(m_uiKey));
                m_inputHook->SetToggleKeys({m_uiKey.second.diKeyCode});
                config.SetKey("UI", "sUiKey", m_uiKey.first.c_str());

                m_keyCode = 0;
                m_inputHook->OnKeyPress.Disconnect(m_keyPressConnection);
            }
        }
    }
}

wchar_t KeybindService::ConvertToUnicode(int32_t aKeyCode) noexcept
{
    wchar_t buffer[10];
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    ToUnicode(aKeyCode, 0, keyboardState, buffer, 10, 0);

    return buffer[0];
}

bool KeybindService::Config::Create() noexcept
{
    if (this->ini.SaveFile(this->path.c_str(), true) == SI_OK)
    {
        spdlog::info("Successfully created {}", kKeybindsFileName);

        this->ini.SetValue("UI", "sUiKey", "F2");
        this->ini.SetValue("UI", "sDebugKey", "F3");

        if (!this->Save())
        {
            spdlog::warn("Failed to save {}", kKeybindsFileName);
            return false;
        }

        return true;
    }

    spdlog::warn("{}: Failed to create {}, using defaults instead", __FUNCTION__, kKeybindsFileName);
    return false;
}

bool KeybindService::Config::Save() const noexcept
{
    return this->ini.SaveFile(this->path.c_str(), true) == SI_OK;
}

bool KeybindService::Config::Load() noexcept
{
    return this->ini.LoadFile(this->path.c_str()) == SI_OK;
}

bool KeybindService::Config::SetKey(const char* acpSection, const char* acpKey, const char* acpValue,
                                   const char* acpDescription) noexcept
{
    this->ini.SetValue(acpSection, acpKey, acpValue, acpDescription);

    return this->Save();
}
