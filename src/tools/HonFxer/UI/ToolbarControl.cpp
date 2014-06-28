/*!
	@file
	@author		Albert Semenov
	@date		08/2010
*/
#include "FxerPch.h"
#include "ToolbarControl.h"
#include "Localise.h"
#include "CommandManager.h"
#include "EditorToolTip.h"

namespace tools
{
	ToolbarControl::ToolbarControl(MyGUI::Widget* _parent) :
		wraps::BaseLayout("FxToolbarControl.layout", _parent)
	{
        addToolButton("ToolNewFile", "New File");
        addToolButton("ToolOpenFile", "Open File");
        addToolButton("ToolSaveFile", "Save File");
        
        addSperator();
        
        addToolButton("ToolUndo", "Undo");
        addToolButton("ToolRedo", "Redo");
        
        addSperator();
        
        addToolButton("ToolCut", "Cut");
        addToolButton("ToolCopy", "Copy");
        addToolButton("ToolPaste", "Paste");
        
        addSperator();
        
        addToolButton("ToolMove", "Move");
        addToolButton("ToolRotate", "Rotate");
        addToolButton("ToolScale", "Scale");
        
        addSperator();
        
        addToolButton("ToolPlay", "Play effect");
        addToolButton("ToolPause", "Pause effect");
        addToolButton("ToolStop", "Stop effect");
        
        addSperator();
        
        addToolButton("ToolDelete", "Delete current selection");
        
        mTipControl = DI_NEW ToolTip();
	}

	ToolbarControl::~ToolbarControl()
	{
        DI_DELETE mTipControl;
	}

	void ToolbarControl::setVisible(bool _value)
	{
		mBar->setVisible(_value);
	}
    
    void ToolbarControl::notifyToolTip(MyGUI::Widget* _sender, const MyGUI::ToolTipInfo& _info)
    {
        auto tipStr = mTooltip[_sender];
        
        if (_info.type == MyGUI::ToolTipInfo::Show)
		{
            mTipControl->show(tipStr, _info.point);
        }
		else if (_info.type == MyGUI::ToolTipInfo::Hide)
		{
			mTipControl->hide();
		}
		else if (_info.type == MyGUI::ToolTipInfo::Move)
		{
			mTipControl->move(_info.point);
		}
    }
    
    void ToolbarControl::addToolButton(const std::string& skin, const std::string& tip)
    {
        auto button = mMainWidget->createWidget<MyGUI::Button>(skin, MyGUI::IntCoord(mCurrentWidth, 7, 25, 25), MyGUI::Align::Default);
        
        if(!tip.empty())
        {
            button->setNeedToolTip(true);
            button->eventToolTip = newDelegate(this, &ToolbarControl::notifyToolTip);
            mTooltip[button] = tip;
        }
        
        mCurrentWidth += 30;
    }
    
    void ToolbarControl::addSperator()
    {
        addToolButton("ToolSperator", "");
    }

} // namespace tools
