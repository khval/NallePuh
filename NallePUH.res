REDC
	WNDW window.class 
		Nalle PUH version 0.3 �2001 Martin Blom <martin@blom.org>
			msgNallePUH
			GADG

	GROUP_2_ID = GROP
		layout.childs
			CHLD layout.gadget
			GROP
			layout.childs
				CHLD
					 layout.gadget
			layout.childs
				GADGCHLD
					GAD_MODE_INFO = string.gadget
			layout.childs
				GADGCHLD
					GAD_PATCH_ROM = checkbox.gadget
					"Patch _ROM"
					msgPatchROM
			layout.childs
				GADGCHLD
					GAD_PATCH_APPS = checkbox.gadget
					"Patch a_pplications"
					msgPatchApplications
			layout.childs
				GADGCHLD
					GAD_TOGGLE_LED = checkbox.gadget
					"Toggle _led on exceptions"
					msgToggleLed
			layout.childs
				GADGCHLD
					GAD_MESSAGES = listbrowser.gadget
					listbrowser.node
					"Activity log area"
					msgActivityLog
		layout.childs
			GROP
			layout.childs
				CHLD
					layout.gadget
				layout.childs
					GADGCHLD
						GAD_MODE_SELECT	= button.gadget 
						"Audio _mode..."
						msgAudioMode
				layout.childs
					GADGCHLD
						GAD_INSTALL
							button.gadget
							"_Install"
							msgInstall
				layout.childs
						GADGCHLD
							GAD_UNINSTALL = button.gadget	
							"_Uninstall"
							msgUninstall
				layout.childs
						GADGCHLD
							GAD_ACTIVATE
								button.gadget
								"_ActivatemsgActivate"
				layout.childs
						GADGCHLD
							GAD_DEACTIVATE
								button.gadget
								"_Deactivate"
								msgDeactivate
				layout.childs
						GADGCHLD
							GAD_TEST
								button.gadget
								"_TestmsgTest"
				layout.childs
						IMGDCHLD
							penmap.image
								pooh11.gif
				layout.childs

