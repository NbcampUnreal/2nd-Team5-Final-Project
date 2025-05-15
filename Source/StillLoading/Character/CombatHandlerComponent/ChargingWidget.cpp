#include "ChargingWidget.h"

#include "Components/ProgressBar.h"

void UChargingWidget::SetChargeProgress(const float Progress) const
{
	if (ChargeProgressBar)
	{
		ChargeProgressBar->SetPercent(Progress);
	}
}
