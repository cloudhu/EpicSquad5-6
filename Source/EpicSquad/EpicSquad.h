// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define DATA_ACCESSOR(DataType,PropertyName) \
FORCEINLINE DataType Get##PropertyName() const { return PropertyName;} \
void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName;}

#define BP_DATA_ACCESSOR(DataType,PropertyName) \
UFUNCTION(BlueprintCallable,BlueprintPure,BlueprintGetter) \
FORCEINLINE DataType Get##PropertyName() const { return PropertyName;} \
UFUNCTION(BlueprintCallable,BlueprintSetter) \
void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName;}