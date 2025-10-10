// Fill out your copyright notice in the Description page of Project Settings.


#include "SLScoreEncodingLibrary.h"

FString USLScoreEncodingLibrary::EncodingScoreToBase36(const int32 InTime, const int32 InCoin)
{
	const uint64 V = InTime * 1000 + InCoin;
	const uint64 T = (A * V + B) % M;
	return ToBase36(T);
}

TArray<int32> USLScoreEncodingLibrary::DecodingBase36ToScore(FString Base36)
{
	TArray<int32> Result;
	
	const uint64 T = FromBase36(Base36);

	constexpr uint64 BMod = B % M;
	
	uint64 Diff;
	if (T >= BMod) Diff = (T - BMod) % M;
	else Diff = (M - (BMod - T) % M) % M;

	const uint64 V =  MI * Diff % M ;

	Result.Add(V / 1000);
	Result.Add(V % 1000);

	return Result;
}

void USLScoreEncodingLibrary::TestEncoding()
{
	for (int i = 0; i< 9999; i++)
	{
		for (int j= 0; j < 999; j++)
		{
			const FString Base36 = EncodingScoreToBase36(i, j);
			TArray<int32> Result = DecodingBase36ToScore(Base36);
			if (Result[0] != i || Result[1] != j)
			{
				UE_LOG(LogTemp, Log, TEXT("디코딩 값이 비정상입니다. Base36 : %s, Result : %d, %d" ), *Base36, Result[0], Result[1]);
			}
		}
	}
}

FString USLScoreEncodingLibrary::ToBase36(uint64 Value)
{
	const TCHAR* Digits = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	FString Result;

	while (Value > 0)
	{
		const int32 Remainder = Value % 36;
		Result.InsertAt(0, Digits[Remainder]);
		Value /= 36;
	}

	if (Result.Len() < TOKEN_LEN)
	{
		const FString Pad = FString::ChrN(TOKEN_LEN - Result.Len(), '0');
		Result = Pad + Result;
	}
	
	return Result.Left(3) + "-" + Result.RightChop(3);
}

uint64 USLScoreEncodingLibrary::FromBase36(const FString& Base36)
{
	FString S = Base36;
	S.ReplaceInline(TEXT("-"), TEXT(""));

	uint64 Result = 0;
	for (const TCHAR C : S)
	{
		int32 Digit = -1;
		if (C >= '0' && C <= '9') Digit = C - '0';
		else if (C >= 'A' && C <= 'Z') Digit = 10 + (C - 'A');
		else checkNoEntry();
		Result = Result * 36 + StaticCast<uint64>(Digit);
	}
	
	return Result;
}
