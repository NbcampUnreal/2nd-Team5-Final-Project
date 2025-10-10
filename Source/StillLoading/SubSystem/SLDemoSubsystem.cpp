// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLDemoSubsystem.h"

void USLDemoSubsystem::StartNewGame()
{
	CoinCount = LastCoin = 0;
	TimeSec = LastTime = 0;

	GetWorld()->GetTimerManager().SetTimer(PlayTimer, this, &ThisClass::IncreaseTime, 1.0f, false);
}

void USLDemoSubsystem::EndCurrentGame()
{
	GetWorld()->GetTimerManager().ClearTimer(PlayTimer);
	//ResetDemoInfo();

	//
}

void USLDemoSubsystem::IncreaseCoin()
{
	++CoinCount;
	CoinDele.Broadcast(CoinCount);
}

int32 USLDemoSubsystem::GetCurrentCoin()
{
	return CoinCount;
}

int32 USLDemoSubsystem::GetCurrentTime()
{
	return TimeSec;
}

void USLDemoSubsystem::PauseTimer(bool bIsCinematic)
{
	if (bIsCinematic)
	{
		bIsCine = true;
	}

	GetWorld()->GetTimerManager().ClearTimer(PlayTimer);
}

void USLDemoSubsystem::ContinueTimer()
{
	bIsCine = false;
	GetWorld()->GetTimerManager().SetTimer(PlayTimer, this, &ThisClass::IncreaseTime, 1.0f, false);
}

bool USLDemoSubsystem::GetIsCinePause() const
{
	return bIsCine;
}

void USLDemoSubsystem::UpdateLastInfo()
{
	LastCoin = CoinCount;
	LastTime = TimeSec;
}

void USLDemoSubsystem::ResetCurrentInfo()
{
	CoinCount = LastCoin;
	TimeSec = LastTime;

	CoinDele.Broadcast(CoinCount);
	TimeDele.Broadcast(TimeSec);
}

void USLDemoSubsystem::IncreaseTime()
{
	++TimeSec;
	TimeDele.Broadcast(TimeSec);

	GetWorld()->GetTimerManager().SetTimer(PlayTimer, this, &ThisClass::IncreaseTime, 1.0f, false);
}

void USLDemoSubsystem::ResetDemoInfo()
{
	CoinCount = 0;
	TimeSec = 0;
	ResetDele.Broadcast();
}