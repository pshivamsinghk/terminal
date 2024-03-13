// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "CodeBlock.h"
#include <LibraryResources.h>

#include "CodeBlock.g.cpp"
#include "RequestRunCommandsArgs.g.cpp"

namespace winrt
{
    namespace MUX = Microsoft::UI::Xaml;
    namespace WUX = Windows::UI::Xaml;
    using IInspectable = Windows::Foundation::IInspectable;
}

namespace winrt::SampleApp::implementation
{
    CodeBlock::CodeBlock(const winrt::hstring& initialCommandlines) :
        _providedCommandlines{ initialCommandlines }
    {
        InitializeComponent();

        if (!_providedCommandlines.empty())
        {
            WUX::Controls::TextBlock b{};
            b.Text(_providedCommandlines);
            b.FontFamily(WUX::Media::FontFamily{ L"Cascadia Code" }); // TODO! get the Style from the control's resources

            CommandLines().Children().Append(b);
        }
    }
    void CodeBlock::_playPressed(const Windows::Foundation::IInspectable&,
                                 const Windows::UI::Xaml::Input::TappedRoutedEventArgs&)
    {
        _block = nullptr;
        OutputBlockContainer().Children().Clear();

        auto args = winrt::make_self<RequestRunCommandsArgs>(Commandlines());
        RequestRunCommands.raise(*this, *args);
    }

    winrt::Microsoft::Terminal::Control::NotebookBlock CodeBlock::OutputBlock()
    {
        return _block;
    }
    void CodeBlock::OutputBlock(const winrt::Microsoft::Terminal::Control::NotebookBlock& block)
    {
        _block = block;
        _block.StateChanged({ get_weak(), &CodeBlock::_blockStateChanged });
        OutputBlockContainer().Children().Append(_block.Control());
        OutputBlockContainer().Visibility(WUX::Visibility::Visible);
    }
    void CodeBlock::_blockStateChanged(const winrt::Microsoft::Terminal::Control::NotebookBlock& sender,
                                       const Windows::Foundation::IInspectable&)
    {
        switch (sender.State())
        {
        case winrt::Microsoft::Terminal::Control::BlockState::Default:
        {
            WUX::VisualStateManager::GoToState(RunButton(), L"Ready", false);
            break;
        }
        case winrt::Microsoft::Terminal::Control::BlockState::Running:
        {
            WUX::VisualStateManager::GoToState(RunButton(), L"Running", false);
            break;
        }
        case winrt::Microsoft::Terminal::Control::BlockState::Finished:
        {
            WUX::VisualStateManager::GoToState(RunButton(), L"AlreadyRan", false);
            break;
        }
        }
    }

}
