 struct InputQueueHook {
    inline static REL::Relocation<void (*)(RE::BSTEventSource<RE::InputEvent*>*, RE::InputEvent* const*)> original;

    static void thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher, RE::InputEvent* const* evns) {
        
        
        bool isInputCapturedByProcessEvent = ProcessEvent(evns);

        if (isInputCapturedByProcessEvent) {
         
            constexpr RE::InputEvent* const dummy[] = {nullptr}; 

            original(dispatcher, dummy);

        // a constexpr of a array of const pointers of type RE::Input event) 
        } 
       else {
            original(dispatcher, evns);
       }
 
    }
}; 