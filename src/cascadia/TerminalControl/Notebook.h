// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "Notebook.g.h"
#include "NotebookBlock.g.h"
#include "EventArgs.h"
#include "../../renderer/base/Renderer.hpp"
#include "../../renderer/uia/UiaRenderer.hpp"
#include "../../cascadia/TerminalCore/Terminal.hpp"
#include "../../cascadia/TerminalCore/BlockRenderData.hpp"
#include "../buffer/out/search.h"

#include "ControlInteractivity.h"
#include "ControlSettings.h"

namespace winrt::Microsoft::Terminal::Control::implementation
{
    struct NotebookBlock : NotebookBlockT<NotebookBlock>
    {
        til::property<Microsoft::Terminal::Control::TermControl> Control{ nullptr };
        til::property<Microsoft::Terminal::Control::BlockState> State{ BlockState::Default };
        til::typed_event<Control::NotebookBlock, Windows::Foundation::IInspectable> StateChanged;

        std::unique_ptr<::Microsoft::Terminal::Core::BlockRenderData> renderData{ nullptr };
        winrt::com_ptr<Microsoft::Terminal::Control::implementation::ControlCore> core{ nullptr };
    };

    struct Notebook : NotebookT<Notebook>
    {
    public:
        Notebook(Control::IControlSettings settings, Control::IControlAppearance unfocusedAppearance, TerminalConnection::ITerminalConnection connection);

        Windows::Foundation::Collections::IVector<Microsoft::Terminal::Control::NotebookBlock> Blocks() const;
        Microsoft::Terminal::Control::NotebookBlock ActiveBlock() const;

        void SendCommands(const winrt::hstring& commands);

        til::typed_event<Control::Notebook, Control::NotebookBlock> NewBlock;

    private:
        std::shared_ptr<::Microsoft::Terminal::Core::Terminal> _terminal{ nullptr };
        TerminalConnection::ITerminalConnection _connection{ nullptr };

        Control::IControlSettings _settings{ nullptr };
        Control::IControlAppearance _unfocusedAppearance{ nullptr };

        // I tried this with true, but then _everything was off by one? Sorta? It was weird.
        // This should probalby be false if we want to do a "fresh" notebook, or to see any output before the first prompt
        bool _gotFirstMark{ false };
        std::vector<winrt::com_ptr<NotebookBlock>> _blocks{};
        std::atomic<bool> _currentlyCreating{ false };
        std::map<til::CoordType, bool> _rowsWithMarks{};
        int64_t _expectedPrompts{ 0 };

        winrt::fire_and_forget _fork(const til::CoordType start);
        void _forkOnUIThread(const til::CoordType start);
        void _createNewBlock(const til::CoordType start);
        winrt::com_ptr<NotebookBlock> _activeBlock() const;

        void _handleNewPrompt(const ::ScrollMark& mark);
    };
}

namespace winrt::Microsoft::Terminal::Control::factory_implementation
{
    BASIC_FACTORY(Notebook);
}
