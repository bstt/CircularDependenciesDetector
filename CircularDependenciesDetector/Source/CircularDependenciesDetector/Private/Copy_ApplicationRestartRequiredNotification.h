// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

// code taken from Developer/SettingsEditor/Private/SettingsEditorModule.cpp

#define LOCTEXT_NAMESPACE "ANA_RestartCallBack"

/** Manages the notification for when the application needs to be restarted due to a settings change */
class FCopy_ApplicationRestartRequiredNotification
{
public:

	void OnRestartRequired()
	{
		FNotificationInfo Info( LOCTEXT("RestartRequiredTitle", "Restart required to apply new settings") );

		// Add the buttons with text, tooltip and callback
		Info.ButtonDetails.Add(FNotificationButtonInfo(
			LOCTEXT("RestartNow", "Restart Now"), 
			LOCTEXT("RestartNowToolTip", "Restart now to finish applying your new settings."), 
			FSimpleDelegate::CreateRaw(this, &FCopy_ApplicationRestartRequiredNotification::OnRestartClicked))
			);
		Info.ButtonDetails.Add(FNotificationButtonInfo(
			LOCTEXT("RestartLater", "Restart Later"), 
			LOCTEXT("RestartLaterToolTip", "Dismiss this notificaton without restarting. Some new settings will not be applied."), 
			FSimpleDelegate::CreateRaw(this, &FCopy_ApplicationRestartRequiredNotification::OnDismissClicked))
			);

		// We will be keeping track of this ourselves
		Info.bFireAndForget = false;

		// Set the width so that the notification doesn't resize as its text changes
		Info.WidthOverride = 300.0f;

		Info.bUseLargeFont = false;
		Info.bUseThrobber = false;
		Info.bUseSuccessFailIcons = false;

		// Launch notification
		NotificationPtr = FSlateNotificationManager::Get().AddNotification(Info);
		TSharedPtr<SNotificationItem> NotificationPin = NotificationPtr.Pin();

		if (NotificationPin.IsValid())
		{
			NotificationPin->SetCompletionState(SNotificationItem::CS_Pending);
		}
	}



private:
	void OnRestartClicked()
	{
		TSharedPtr<SNotificationItem> NotificationPin = NotificationPtr.Pin();
		if (NotificationPin.IsValid())
		{
			NotificationPin->SetText(LOCTEXT("RestartingNow", "Restarting..."));
			NotificationPin->SetCompletionState(SNotificationItem::CS_Success);
			NotificationPin->ExpireAndFadeout();
			NotificationPtr.Reset();
		}

		FUnrealEdMisc::Get().RestartEditor(false);
	}

	void OnDismissClicked()
	{
		TSharedPtr<SNotificationItem> NotificationPin = NotificationPtr.Pin();
		if (NotificationPin.IsValid())
		{
			NotificationPin->SetText(LOCTEXT("RestartDismissed", "Restart Dismissed..."));
			NotificationPin->SetCompletionState(SNotificationItem::CS_None);
			NotificationPin->ExpireAndFadeout();
			NotificationPtr.Reset();
		}
	}

	/** Used to reference to the active restart notification */
	TWeakPtr<SNotificationItem> NotificationPtr;
};

#undef LOCTEXT_NAMESPACE
