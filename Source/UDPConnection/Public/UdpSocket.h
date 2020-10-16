#pragma once

#include "GameFramework/Actor.h"
#include "Networking.h"
#include "UdpSocket.generated.h"

UCLASS()
class AUdpSocket : public AActor
{
	GENERATED_UCLASS_BODY()

#pragma region Sender
public:
	TSharedPtr<FInternetAddr> RemoteAddr;
	FSocket* SenderSocket;

	UFUNCTION(BlueprintCallable, Category = "Custom Process Creater Udp Socket")
		bool UdpSocket_SendString(FString ToSend);

	UFUNCTION(BlueprintCallable, Category = "Custom Process Creater Udp Socket")
		bool StartUDPSender(const FString& SocketName, const FString& IPAddr, const int32 Port);
#pragma endregion



#pragma region Receiver
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Process Creater Udp Socket", DisplayName = "On Data Received")
	void BPEvent_DataReceived(const FString &ReceivedData);

	FSocket* ListenSocket;

	FUdpSocketReceiver* UDPReceiver = nullptr;
	
	void ReceiveData(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

	
	UFUNCTION(BlueprintCallable, Category = "Custom Process Creater Udp Socket")
	bool StartUDPReceiver(const FString& SocketName, const int32 Port, int32 WaitTimeInMilis = 100);
	
#pragma endregion



#pragma region Actor
public:
	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion



#pragma region Helper
private:
	FString BytesToStringFixed(const uint8* In, int32 Count);
#pragma endregion


};