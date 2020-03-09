using System;

namespace VirtualDesktops
{
    public class WindowChangedDesktopEventArgs : EventArgs
    {
        public ITopLevelWindow Window;
    }

    public class VirtualDesktopRemovedEventArgs : EventArgs
    {
        public VirtualDesktop RemovedDesktop;

        // I assume this is where all the windows from the old desktop go?
        public VirtualDesktop NewTargetDesktop; 
    }

    // This seems like it could just be a shoulder-tap; no need to pass
    // in the args.
    public class VirtualDesktopChangedEventArgs : EventArgs
    {
        public VirtualDesktop OriginalDesktop;
        public VirtualDesktop NewDesktop;
    }

    public class VirtualDesktopAddedEventArgs : EventArgs
    {
        public VirtualDesktop Desktop;
    }

    public class VirtualDesktopManager
    {
        // Current vs. GetDefault() vs. whatever....
        public static VirtualDesktopManager GetDefault() { return new VirtualDesktopManager(); }

        // I assume this was for switching desktops?
        public event TypedEventHandler<VirtualDesktopManager, VirtualDesktopChangedEventArgs> CurrentDesktopChanged;
        
        // Created a virtual destktop, VirtualDesktopCreated
        public event TypedEventHandler<VirtualDesktopManager, VirtualDesktopAddedEventArgs> VirtualDesktopAdded;
        
        // destroy successed, VirtualDesktopDestroyed
        public event TypedEventHandler<VirtualDesktopManager, VirtualDesktopRemovedEventArgs> VirtualDesktopRemoved;
        // begin destroying, VirtualDesktopDestroyBegin

        // A "pre"-event like 'Removing' only makes sense if the app can cancel the event; otherwise, what
        // are they going to do with it? And if you don't have the 'ing' then you don't need the 'failed' either.
        //public event TypedEventHandler<VirtualDesktopManager, DestroyVirtualDesktopEventArgs> VirtualDesktopRemoving;
        // failed to destroy, VirtualDesktopDestroyFailed
        //public event TypedEventHandler<VirtualDesktopManager, DestroyVirtualDesktopEventArgs> DestroyFailed;
        
        // Should there be a 'Added' / 'Removed' pair on VirtualDesktop instead? The Desktop has the list of
        // windows on it, but then you need to listen over here to know when the list changes. And there doesn't
        // appear to be an event for "a new window was created and it landed on this desktop"
        public event TypedEventHandler<VirtualDesktopManager, WindowChangedDesktopEventArgs> WindowChangedDesktops;

        // Count is redundant when you have the list of desktops
        //public int Count{ get { return 0; } }

        public IList<VirtualDesktop> VirtualDesktops { get; }

        // There is no event to say when 'current' changes... unless that'sd what the Changed event is?
        public VirtualDesktop CurrentDesktop { get { return new VirtualDesktop(); } }
        
        // I would consider leaving out this trivial overload; it saves you about five keystrokes with IntelliSense.
        public VirtualDesktop GetAdjacentDesktop(VirtualDesktopAdjacentDirection direction) { return AdjacentDesktop(direction, CurrentDesktop); }

        // Re-ordered params so they are in the right order w.r.t. other overload
        public VirtualDesktop GetAdjacentDesktop(VirtualDesktopAdjacentDirection direction, VirtualDesktop desktopReference) { return new VirtualDesktop(); }

        // Added 'Try' since I assume maybe it can fail? Does it need an Async version? 
        // (note current guidance is that if you think you need async, add both sync and async)
        public bool TrySwitchToDesktop(VirtualDesktop newDesktop) { }

        // Do these need to be a 'Try'? Can they fail? Async?
        public VirtualDesktop AddDesktop(VirtualDesktopAdjacentDirection direction) { return CreateVirtualDesktop(direction, CurrentDesktop); }
        public VirtualDesktop AddDesktop(VirtualDesktopAdjacentDirection direction, VirtualDesktop desktopReference) { return new VirtualDesktop(); }

        public bool CanRemoveDesktop(VirtualDesktop desktop) { return true; }

        // Added 'Try'; maybe want an async version as well.
        public bool TryRemoveDesktop(VirtualDesktop desktop, VirtualDesktop newTargetDesktop) { }

        public VirtualDesktop GetById(String virtualDesktopId) { return new VirtualDesktop(); }

        // Kill this. "Current" is dangerous because it can change over time. 
        //public bool IsWindowOnCurrentDesktop(ITopLevelWindow window) { return CurrentDesktop.IsWindowPresent(window); }

        public void MoveWindowToDesktop(ITopLevelWindow window, VirtualDesktop desktop) { desktop.TransferWindow(window); }
        public void MoveApplicationToDesktop(IApplicationId appId, VirtualDesktop desktop) { desktop.TransferApplication(app); }

        // Why are these on the Manager and not on a VirtualDesktop? I assume you're pinning them to a specific desktop?
        // Also, no events for when the user does this via the UI?
        // Do any of these need to be 'Try'?
        public bool PinApplication(IApplicationId appId)
        { return true; }

        public bool PinWindow(ITopLevelWindow window)
        { return true; }

        public bool UnpinApplication(IApplicationId appId)
        { return true; }

        public bool UnpinWindow(ITopLevelWindow window)
        { return true; }

        public bool IsApplicationPinned(IApplicationId appId)
        { return true; }

        public bool IsWindowPinned(ITopLevelWindow window)
        { return true; }

        // Where is the "Show on all desktops" feature? Is there no API for that?
    }

    public class VirtualDesktop
    {
        // Visible or "on this desktop"?
        // Visibility can be ambiguous (eg, window is on this desktop, but is minimized)
        public bool IsWindowVisible(ITopLevelWindow window) { return true; }

        // How persistent are these Ids?
        // It would only be meaningful if Desktop Ids persisted across reboots (otherwise
        // you would just use the object).
        public String Id { get; }

        // What is 'present' vs. 'visible'?
        public bool IsWindowPresent(ITopLevelWindow window) { return true; }
        public bool IsApplicationPresent(IApplicationId app) { return true; }

        // Get rid of these; manager can do it
        //public void TransferWindow(ITopLevelWindow window) { }
        //public void TransferApplication(IApplicationId app) { }

        // Are these just things that are "on" the desktop? See comment above about eventing and the fact
        // that you don't know when these change.
        public IList<IApplicationId> Applications;
        public IList<ITopLevelWindow> Windows;
    }
    
    public interface ITopLevelWindow
    {
        // TODO... probably just a marker with a set of
        // Win32 / WinRT factories.
    }

    public interface IApplicationId
    {
        // Wrapper of AUMID or PFN or something else? 
        // Maybe just a string?
    }

    public enum VirtualDesktopAdjacentDirection
    {
        Left = 3,
        Right = 4
    };    
}
