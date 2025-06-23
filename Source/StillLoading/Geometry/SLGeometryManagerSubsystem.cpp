// Fill out your copyright notice in the Description page of Project Settings.


#include "Geometry/SLGeometryManagerSubsystem.h"

#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "GeometryCollection/GeometryCollection.h"

void USLGeometryManagerSubsystem::RegisterGeometryCollection(AGeometryCollectionActor* GCActor)
{
	if (GCActor && !RegisteredGCActors.Contains(GCActor))
	{
		RegisteredGCActors.Add(GCActor);
	}
}

void USLGeometryManagerSubsystem::UnregisterGeometryCollection(AGeometryCollectionActor* GCActor)
{
	RegisteredGCActors.Remove(GCActor);
}

void USLGeometryManagerSubsystem::StopAllGeometryChunks()
{

	if (RegisteredGCActors.IsEmpty())
	{
		return;
	}

	for (TWeakObjectPtr<AGeometryCollectionActor> WeakActor : RegisteredGCActors)
	{
		if (AGeometryCollectionActor* GCActor = WeakActor.Get())
		{
			UGeometryCollectionComponent* GCComp = GCActor->GetGeometryCollectionComponent();
			if (!GCComp || !GCComp->GetRestCollection())
				continue;

			const FGeometryCollection* RestCollection = GCComp->GetRestCollection()->GetGeometryCollection().Get();
			if (!RestCollection)
				continue;

			const TManagedArray<FString>& BoneNames = RestCollection->BoneName;
			int32 NumBones = BoneNames.Num();



			for (int32 i = 0; i < NumBones; ++i)
			{
				FName BoneName = FName(*BoneNames[i]);

				bool bSimulating = GCComp->IsSimulatingPhysics(BoneName);
				FVector Velocity = GCComp->GetPhysicsLinearVelocity(BoneName);
				FVector AngularVelocity = GCComp->GetPhysicsAngularVelocityInDegrees(BoneName);

				if (bSimulating)
				{
					GCComp->SetPhysicsLinearVelocity(FVector::ZeroVector, false, BoneName);
					GCComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector, false, BoneName);
				}
			}
		}
	}
}
