///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 - 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include "windows.h"
#ifdef _DEBUG
// debug header for leak detection
// http://www.codeproject.com/KB/applications/visualleakdetector.aspx
#include "vld.h"
#endif // _DEBUG
#endif

#include "JuceHeader.h"
#include "tcat_dice_pal_system.h"


template<bool sub_items=false> class tvi_static: public TreeViewItem
{
public:
	tvi_static(std::string name): name(name) {}
    bool mightContainSubItems() { return sub_items; }
	void paintItem (Graphics& g, int width, int height)
	{
		// if this item is selected, fill it with a background colour..
		if (isSelected())
			g.fillAll (Colours::blue.withAlpha (0.3f));
		g.setColour (Colours::black);
		g.setFont (height * 0.9f);
		g.drawText (name.c_str(), 4, 0, width - 4, height, Justification::centredLeft, true);
	}
    virtual int getItemHeight() const	{ return 14; }
//private:
	std::string name;
};



class device_view: public tcat::dice::device
{
	typedef tcat::dice::device super;
	enum
	{
		kItemType,
		kItemGuid,
		kItemState,
		kItemIdx,
		kItemClockLock,
		kItemLockTx,
		kItemLockRx,
		kItemIo,
		
		kItems
	};
	enum
	{
		kItemIoRx,
		kItemIoTx,
		kItemIos
	};
	
public:
	device_view()
	{
		dv_root = 0;
	}
	virtual ~device_view()
	{
		if (dv_root)
		{
			const MessageManagerLock mmLock;
			tcat::dice::lock lock(this);
			dv_root->clearSubItems();
		}
	}
	void	create(tcat::dice::bus_impl* bus, const tcat::dice::DEVICE_INFO& info) throw(tcat::exception::base)
	{
		super::create(bus,info);
		dv_root = new tvi_static<true>("[dev]");
		dv_root->setOpen(true);
		dv_root->addSubItem(new tvi_static<false>(type()));
		dv_root->addSubItem(new tvi_static<false>("guid: " + tcat::hex_to_string<tcat::uint64>(guid64())));
		dv_root->addSubItem(new tvi_static<false>("[state]"));
		dv_root->addSubItem(new tvi_static<false>("[idx]"));
		dv_root->addSubItem(new tvi_static<false>("[clk/lck]"));
		dv_root->addSubItem(new tvi_static<false>("[tx lck]"));
		dv_root->addSubItem(new tvi_static<false>("[rx lck]"));
		dv_root->addSubItem(new tvi_static<true>("i/o"));
		dv_root->getSubItem(kItemIo)->addSubItem(new tvi_static<false>("[rx]"));
		dv_root->getSubItem(kItemIo)->addSubItem(new tvi_static<false>("[tx]"));
	}
	void	update_state()
	{
		super::update_state();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(dv_root->getSubItem(kItemState));
		std::string device_state[] = {"invalid","active","low resourcses","lock failed","version conflict","disabled","unknown error","removed","already locked"};
		item->name = "state: " + device_state[state()];
		item->treeHasChanged();
	}
	void	update_idx()
	{
		super::update_clock();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(dv_root->getSubItem(kItemIdx));
		item->name = "idx: " + tcat::to_string<tcat::uint32>(idx());
		item->treeHasChanged();
	}
	void	update_nickname()
	{
		super::update_nickname();
		const MessageManagerLock jlock;
		dv_root->name = nickname();
// bkbk		dynamic_cast<tvi_static<true>*>(dv_root)->name = nickname();
		dv_root->treeHasChanged();
//		m_bus->update_master();
	}
	void	update_clocklock()
	{
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(dv_root->getSubItem(kItemClockLock));
		if (locked())
			item->name = "locked to: " + tcat::to_string<tcat::uint32>(clock()) + "Hz";
		else
			item->name = "unlocked";
		item->treeHasChanged();
	}
	void	update_clock()
	{
		super::update_clock();
		update_clocklock();
	}
	void	update_locked()
	{
		super::update_locked();
		update_clocklock();
	}
	void	update_rx()	throw(tcat::exception::base)
	{
		super::update_rx();
		const MessageManagerLock jlock;
		tvi_static<true>* item = dynamic_cast<tvi_static<true>*>(dv_root->getSubItem(kItemIo));
		tvi_static<true>* branch = new tvi_static<true>(
			tcat::to_string<size_t>(channels(false).size()) + " audio/in channels, " + 
			tcat::to_string<tcat::uint32>(midi_ports(false)) + " MIDI/in ports");
		branch->setOpen(item->getSubItem(kItemIoRx)->isOpen());
		item->removeSubItem(kItemIoRx);
		item->addSubItem(branch, kItemIoRx);
		for (channel_list::const_iterator chn=channels(false).begin(); chn!=channels(false).end(); chn++)
			branch->addSubItem(
				new tvi_static<false>(
					(*chn) + 
					" [isoc:" + tcat::to_string<tcat::uint32>((*chn).isoc()) +
					", AVS::" + tcat::to_string<tcat::uint32>((*chn).avs()) + "]"));
	}
	void	update_tx() throw(tcat::exception::base)
	{
		super::update_tx();
		const MessageManagerLock jlock;
		tvi_static<true>* item = dynamic_cast<tvi_static<true>*>(dv_root->getSubItem(kItemIo));
		tvi_static<true>* branch = new tvi_static<true>(
			tcat::to_string<size_t>(channels(true).size()) + " audio/out channels, " + 
			tcat::to_string<tcat::uint32>(midi_ports(true)) + " MIDI/out ports");
		branch->setOpen(item->getSubItem(kItemIoTx)->isOpen());
		item->removeSubItem(kItemIoTx);
		item->addSubItem(branch, kItemIoTx);
		for (channel_list::const_iterator chn=channels(true).begin(); chn!=channels(true).end(); chn++)
			branch->addSubItem(
				new tvi_static<false>(
					(*chn) + 
					" [isoc:" + tcat::to_string<tcat::uint32>((*chn).isoc()) +
					", AVS::" + tcat::to_string<tcat::uint32>((*chn).avs()) + "]"));
	}
	void	update_tx_locked()
	{
		super::update_tx_locked();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(dv_root->getSubItem(kItemLockTx));
		item->name = 
			"tx0: " + std::string(tx_locked(0)?"":"un") + "locked, " +
			"tx1: " + std::string(tx_locked(1)?"":"un") + "locked";
		item->treeHasChanged();
	}
	void	update_rx_locked()
	{
		super::update_rx_locked();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(dv_root->getSubItem(kItemLockRx));
		item->name = "lock:";
		for (uint32 bit=0; bit<tcat::dice::kExStatusItems; bit++)
		{
			std::string lock_names[] = {"Aes0",	"Aes1", "Aes2", "Aes3", "Adat", "Tdif", "Arx0", "Arx1", "Arx2", "Arx3"}; 
			int flag = 1<<bit;
			if (rx_locked() & flag)
				item->name += " " + lock_names[bit];
		}
		item->treeHasChanged();
	}
	tvi_static<true>* dv_root; 
};



class bus_view:	public tcat::dice::bus<device_view>
{
	typedef tcat::dice::bus<device_view> super;
	enum
	{
		kItemVersion,
		kItemClock,
		kItemSync,
		kItemOpMode,
		kItemMaster,
		kItemDevice,
		
		kItems
	};
public:
	bus_view()
	{
		bv_root = 0;
	}
	virtual ~bus_view()
	{
		if (bv_root)
		{
			const MessageManagerLock mmLock;
			tcat::dice::lock lock(this);
			bv_root->clearSubItems();
		}
	}
	void	create(tcat::dice::system_impl* system, const ID id) throw(tcat::exception::base)
	{
		super::create(system,id);
		bv_root = new tvi_static<true>(kTCAT_DICE_USR_STR);
		bv_root->setOpen(true);
		bv_root->addSubItem(new tvi_static<false>("[drv ver]"));
		bv_root->addSubItem(new tvi_static<false>("[clk]"));
		bv_root->addSubItem(new tvi_static<false>("[snc]"));
		bv_root->addSubItem(new tvi_static<false>("[op_mode]"));
		bv_root->addSubItem(new tvi_static<false>("[mstr]"));
		bv_root->addSubItem(new tvi_static<true>("[dev]"));
		bv_root->getSubItem(kItemDevice)->setOpen(true);
	}
	void	update_clock()
	{
		super::update_clock();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(bv_root->getSubItem(kItemClock));
		item->name = "clock: " + super::clock_name(super::clock());
		item->treeHasChanged();
	}
	void	update_sync()
	{
		super::update_sync();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(bv_root->getSubItem(kItemSync));
		item->name = "sync: " + super::sync_name(super::sync());
		item->treeHasChanged();
	}
	void	update_op_mode()
	{
		super::update_clock();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(bv_root->getSubItem(kItemOpMode));
		item->name = "op_mode: " + tcat::to_string<tcat::uint32>(super::op_mode());
		item->treeHasChanged();
	}
	void	update_master()
	{
		super::update_master();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(bv_root->getSubItem(kItemMaster));
		tcat::dice::reference<device_view> device = super::find(super::master());
		item->name = "master: ";
		if (device)
			item->name += device->nickname();
		else 
			item->name += "n/a";
		item->treeHasChanged();
	}
	void	mount()
	{
		super::mount();
		const MessageManagerLock jlock;
		tvi_static<false>* item = dynamic_cast<tvi_static<false>*>(bv_root->getSubItem(kItemVersion));
		item->name = "driver version: " + 
			tcat::to_string<tcat::uint32>(TCAT_VERSION32_MAJOR(driver_version())) + "." +
			tcat::to_string<tcat::uint32>(TCAT_VERSION32_MINOR(driver_version())) + "." +
			tcat::to_string<tcat::uint32>(TCAT_VERSION32_SUB(driver_version())) + "." +
			tcat::to_string<tcat::uint32>(TCAT_VERSION32_BUILD(driver_version())) +
			(driver_version()&kTCAT_DICE_VERSION_MASK_BUILDSERVER?"":" local") +
			(driver_version()&kTCAT_DICE_VERSION_MASK_BETA?" beta":"") +
			(driver_version()&kTCAT_DICE_VERSION_MASK_INSTRUMENTED?" instrumented":"");
		item->treeHasChanged();
		update_master();
	}
	void	update_devicelist() throw(tcat::exception::base)
	{
		tcat::dice::lock lock(this);
		const MessageManagerLock jlock;
		tvi_static<true>* item = dynamic_cast<tvi_static<true>*>(bv_root->getSubItem(kItemDevice));
		while (item->getNumSubItems())
			item->removeSubItem(0,false);
		super::update_devicelist();
		item->name = tcat::to_string<size_t>(size()) + " device(s)";
		for (iterator device=begin(); device!=end(); device++)
			item->addSubItem((*device)->dv_root);
		item->treeHasChanged();
	}
	tvi_static<true>* bv_root; 
};



class system_view: 
	public TreeView, 
	public tcat::dice::system<bus_view>
{
	typedef tcat::dice::system<bus_view> super;
public:
	system_view()
	{
		sv_root = NULL;
	}
	virtual ~system_view()
	{
		const MessageManagerLock mmLock;
		tcat::dice::lock lock(this);
		if (sv_root)
		{
			while (sv_root->getNumSubItems())
			{
				sv_root->removeSubItem(0, true);
			}
		}
		deleteRootItem();
	}
	virtual void		update_buslist() throw(tcat::exception::base)
	{
		super::update_buslist();
		const MessageManagerLock jlock;
		tcat::dice::lock lock(this);
		if (size())
		{
			sv_root = ((*begin())->bv_root);
			setRootItem (sv_root);
		}
		else
			setRootItem (0);		
//		treeHasChanged();
	}

	TreeViewItem * sv_root;
};
typedef tcat::dice::reference<system_view> system_view_ref;



class cpl_comp: public Component
{
	system_view_ref	system;
public:
	cpl_comp()
	{
		try
		{
			system = system_view::static_create<system_view>();
			addAndMakeVisible (system);
		}
		catch (tcat::exception::base xptn)
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon, xptn.usr_msg().c_str(), xptn.dbg_msg().c_str(), "It's a pity.");
			throw xptn;
		}
		catch(...)
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon, "General Application Error", "The Application needs to close.", "Commiseration");
			throw 0;
		}
	}

	virtual ~cpl_comp()
	{
		if (system)
		{
			system->close();
//			system->clearTree();
		}
	}

	void resized()
	{
		if (system)
		{
			system->setBoundsRelative (0, 0, 1, 1);
		}
	}
};



//==============================================================================
/** This is the top-level window that we'll pop up. Inside it, we'll create and
    show a cpl_comp component.
*/
class cpl_win: public DocumentWindow
{
public:
    cpl_win(): DocumentWindow (kTCAT_DICE_USR_STR " (" kTCAT_DICE_VERSION_EXTENDED_STR ")", Colours::lightgrey, DocumentWindow::allButtons, true)
    {
        setContentOwned (new cpl_comp(), false);
        setVisible (true);
        setResizable (true,true);
        centreWithSize (400, 600);
    }
    void closeButtonPressed()
    {
		JUCEApplication::getInstance()->systemRequestedQuit();
    }
};



//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
    the initialisation and shutdown of the whole application.
*/
class cpl: public JUCEApplication
{
    /* Important! NEVER embed objects directly inside your JUCEApplication class! Use
       ONLY pointers to objects, which you should create during the initialise() method
       (NOT in the constructor!) and delete in the shutdown() method (NOT in the
       destructor!)

       This is because the application object gets created before Juce has been properly
       initialised, so any embedded objects would also get constructed too soon.
   */
    cpl_win* window;
public:
	cpl(): window(0) {}
	void initialise (const String& commandLine)	{ window = new cpl_win(); }
	void shutdown()								{ if (window) delete window; }
	const String getApplicationName()			{ return kTCAT_DICE_USR_STR; }
	const String getApplicationVersion()		{ return kTCAT_DICE_VERSION_EXTENDED_STR;	}
	bool moreThanOneInstanceAllowed()			{ return true; }
//	void anotherInstanceStarted (const String& commandLine)	{}
};


//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION (cpl)
