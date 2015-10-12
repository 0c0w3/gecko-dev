/* -*- Mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 40 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_bluetooth_bluetoothadapter_h__
#define mozilla_dom_bluetooth_bluetoothadapter_h__

#include "mozilla/Attributes.h"
#include "mozilla/DOMEventTargetHelper.h"
#include "mozilla/dom/BluetoothMapParametersBinding.h"
#include "mozilla/dom/BluetoothPbapParametersBinding.h"
#include "BluetoothCommon.h"
#include "BluetoothPropertyContainer.h"
#include "nsCOMPtr.h"

namespace mozilla {
namespace dom {
class File;
class DOMRequest;
struct MediaMetaData;
struct MediaPlayStatus;
}
}

BEGIN_BLUETOOTH_NAMESPACE

class BluetoothDevice;
class BluetoothSignal;
class BluetoothNamedValue;
class BluetoothValue;

class BluetoothAdapter : public DOMEventTargetHelper
                       , public BluetoothSignalObserver
                       , public BluetoothPropertyContainer
{
public:
  NS_DECL_ISUPPORTS_INHERITED

  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS_INHERITED(BluetoothAdapter,
                                                         DOMEventTargetHelper)

  static already_AddRefed<BluetoothAdapter>
  Create(nsPIDOMWindow* aOwner, const BluetoothValue& aValue);

  void Notify(const BluetoothSignal& aParam);

  void Unroot();
  virtual void SetPropertyByValue(const BluetoothNamedValue& aValue) MOZ_OVERRIDE;

  virtual void DisconnectFromOwner() MOZ_OVERRIDE;

  void GetAddress(nsString& aAddress) const
  {
    aAddress = mAddress;
  }

  uint32_t
  Class() const
  {
    return mClass;
  }

  void
  GetName(nsString& aName) const
  {
    aName = mName;
  }

  bool
  Discovering() const
  {
    return mDiscovering;
  }

  bool
  Discoverable() const
  {
    return mDiscoverable;
  }

  uint32_t
  DiscoverableTimeout() const
  {
    return mDiscoverableTimeout;
  }

  void GetDevices(JSContext* aContext, JS::MutableHandle<JS::Value> aDevices,
                  ErrorResult& aRv);
  void GetUuids(JSContext* aContext, JS::MutableHandle<JS::Value> aUuids,
                ErrorResult& aRv);

  already_AddRefed<mozilla::dom::DOMRequest>
    SetName(const nsAString& aName, ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    SetDiscoverable(bool aDiscoverable, ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    SetDiscoverableTimeout(uint32_t aTimeout, ErrorResult& aRv);
  already_AddRefed<DOMRequest> StartDiscovery(ErrorResult& aRv);
  already_AddRefed<DOMRequest> StopDiscovery(ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    Pair(const nsAString& aDeviceAddress, ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    Unpair(const nsAString& aDeviceAddress, ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    GetPairedDevices(ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    SetPinCode(const nsAString& aDeviceAddress, const nsAString& aPinCode,
               ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    SetPasskey(const nsAString& aDeviceAddress, uint32_t aPasskey,
               ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    SetPairingConfirmation(const nsAString& aDeviceAddress, bool aConfirmation,
                           ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    SetAuthorization(const nsAString& aDeviceAddress, bool aAllow,
                     ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    Connect(BluetoothDevice& aDevice,
            const Optional<uint16_t>& aServiceUuid, ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    Disconnect(BluetoothDevice& aDevice,
               const Optional<uint16_t>& aServiceUuid,
               ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    IsConnected(const uint16_t aServiceUuid,
                ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    GetConnectedDevices(uint16_t aServiceUuid, ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    SendFile(const nsAString& aDeviceAddress, File& aBlob,
             ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    StopSendingFile(const nsAString& aDeviceAddress, ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    ConfirmReceivingFile(const nsAString& aDeviceAddress, bool aConfirmation,
                         ErrorResult& aRv);

  already_AddRefed<DOMRequest> ConnectSco(ErrorResult& aRv);
  already_AddRefed<DOMRequest> DisconnectSco(ErrorResult& aRv);
  already_AddRefed<DOMRequest> IsScoConnected(ErrorResult& aRv);

  already_AddRefed<DOMRequest> AnswerWaitingCall(ErrorResult& aRv);
  already_AddRefed<DOMRequest> IgnoreWaitingCall(ErrorResult& aRv);
  already_AddRefed<DOMRequest> ToggleCalls(ErrorResult& aRv);

  already_AddRefed<DOMRequest>
    SendMediaMetaData(const MediaMetaData& aMediaMetaData, ErrorResult& aRv);
  already_AddRefed<DOMRequest>
    SendMediaPlayStatus(const MediaPlayStatus& aMediaPlayStatus, ErrorResult& aRv);

  IMPL_EVENT_HANDLER(devicefound);
  IMPL_EVENT_HANDLER(discoverystatechanged);
  IMPL_EVENT_HANDLER(a2dpstatuschanged);
  IMPL_EVENT_HANDLER(hfpstatuschanged);
  IMPL_EVENT_HANDLER(pairedstatuschanged);
  IMPL_EVENT_HANDLER(pullphonebookreq);
  IMPL_EVENT_HANDLER(pullvcardentryreq);
  IMPL_EVENT_HANDLER(pullvcardlistingreq);
  IMPL_EVENT_HANDLER(requestmediaplaystatus);
  IMPL_EVENT_HANDLER(scostatuschanged);
  IMPL_EVENT_HANDLER(mapfolderlistingreq);
  IMPL_EVENT_HANDLER(mapmessageslistingreq);
  IMPL_EVENT_HANDLER(mapgetmessagereq);
  IMPL_EVENT_HANDLER(mapsetmessagestatusreq);
  IMPL_EVENT_HANDLER(mapsendmessagereq);
  IMPL_EVENT_HANDLER(mapmessageupdatereq);

  nsPIDOMWindow* GetParentObject() const
  {
     return GetOwner();
  }

  virtual JSObject*
    WrapObject(JSContext* aCx) MOZ_OVERRIDE;

private:
  BluetoothAdapter(nsPIDOMWindow* aOwner, const BluetoothValue& aValue);
  ~BluetoothAdapter();

  void Root();

  already_AddRefed<mozilla::dom::DOMRequest>
    StartStopDiscovery(bool aStart, ErrorResult& aRv);
  already_AddRefed<mozilla::dom::DOMRequest>
    PairUnpair(bool aPair, const nsAString& aDeviceAddress, ErrorResult& aRv);

  /**
   * Handle PULL_PHONEBOOK_REQ_ID bluetooth signal.
   *
   * @param aValue [in] Properties array of the PBAP request.
   *                    The array should contain few properties:
   *                    - nsString   'name'
   *                    - bool       'format'
   *                    - uint32_t[] 'propSelector'
   *                    - uint32_t   'maxListCount'
   *                    - uint32_t   'listStartOffset'
   *                    - uint32_t[] 'vCardSelector_AND'
   *                    - uint32_t[] 'vCardSelector_AND'
   */
  void HandlePullPhonebookReq(const BluetoothValue& aValue);

  /**
   * Handle PULL_VCARD_ENTRY_REQ_ID bluetooth signal.
   *
   * @param aValue [in] Properties array of the PBAP request.
   *                    The array should contain few properties:
   *                    - nsString   'name'
   *                    - bool       'format'
   *                    - uint32_t[] 'propSelector'
   */
  void HandlePullVCardEntryReq(const BluetoothValue& aValue);

  /**
   * Handle PULL_VCARD_LISTING_REQ_ID bluetooth signal.
   *
   * @param aValue [in] Properties array of the PBAP request.
   *                    The array should contain few properties:
   *                    - nsString   'name'
   *                    - nsString   'order'
   *                    - nsString   'searchText'
   *                    - nsString   'searchKey'
   *                    - uint32_t   'maxListCount'
   *                    - uint32_t   'listStartOffset'
   *                    - uint32_t[] 'vCardSelector_AND'
   *                    - uint32_t[] 'vCardSelector_AND'
   */
  void HandlePullVCardListingReq(const BluetoothValue& aValue);

  /**
   * Get a Sequence of vCard properies from a BluetoothValue. The name of
   * BluetoothValue must be propSelector, vCardSelector_OR or vCardSelector_AND.
   *
   * @param aValue [in] a BluetoothValue with 'TArrayOfuint32_t' type
   *                    The name of BluetoothValue must be 'propSelector',
   *                    'vCardSelector_OR' or 'vCardSelector_AND'.
   */
  Sequence<vCardProperties> getVCardProperties(const BluetoothValue &aValue);

   /**
   * Handle "MapFolderListing" bluetooth signal.
   *
   * @param aValue [in] Properties array of the MAP request.
   *                    The array should contain a few properties:
   *                    - uint32_t  'MaxListCount'
   *                    - uint32_t  'ListStartOffset'
   */
  void HandleMapFolderListing(const BluetoothValue& aValue);

  /**
   * Handle "MapMessageListing" bluetooth signal.
   *
   * @param aValue [in] Properties array of the MAP request.
   *                    The array should contain a few properties:
   *                    - uint32_t  'MaxListCount'
   *                    - uint32_t  'ListStartOffset'
   *                    - uint32_t  'SubjectLength'
   *                    - uint32_t  'ParameterMask'
   *                    - uint32_t  'FilterMessageType'
   *                    - nsString  'FilterPeriodBegin'
   *                    - nsString  'FilterPeriodEnd'
   *                    - uint32_t  'FilterReadStatus'
   *                    - nsString  'FilterRecipient'
   *                    - nsString  'FilterOriginator'
   *                    - uint32_t  'FilterPriority'
   */
  void HandleMapMessagesListing(const BluetoothValue& aValue);

  /**
   * Handle "MapGetMessage" bluetooth signal.
   *
   * @param aValue [in] Properties array of the MAP request.
   *                    The array should contain a few properties:
   *                    - bool       'Attachment'
   *                    - nsString   'Charset'
   */
  void HandleMapGetMessage(const BluetoothValue& aValue);

  /**
   * Handle "MapSetMessageStatus" bluetooth signal.
   *
   * @param aValue [in] Properties array of the scanned device.
   *                    The array should contain a few properties:
   *                    - long       'HandleId'
   *                    - uint32_t   'StatusIndicator'
   *                    - bool       'StatusValue'
   */
  void HandleMapSetMessageStatus(const BluetoothValue& aValue);

  /**
   * Handle "MapSendMessage" bluetooth signal.
   *
   * @param aValue [in] Properties array of the scanned device.
   *                    The array should contain a few properties:
   *                    - nsString    'Recipient'
   *                    - nsString    'MessageBody'
   *                    - uint32_t    'Retry'
   */
  void HandleMapSendMessage(const BluetoothValue& aValue);

  /**
   * Handle "MapMessageUpdate" bluetooth signal.
   *
   * @param aValue [in] Properties array of the scanned device.
   *                    - nsString     'MASInstanceID'
   */
  void HandleMapMessageUpdate(const BluetoothValue& aValue);

  /**
   * Get a Sequence of ParameterMask from a BluetoothValue. The name of
   * BluetoothValue must be parameterMask.
   *
   * @param aValue [in] a BluetoothValue with 'TArrayOfuint32_t' type
   *                    The name of BluetoothValue must be 'parameterMask'.
   */
  Sequence<ParameterMask> GetParameterMask(const BluetoothValue &aValue);

  JS::Heap<JSObject*> mJsUuids;
  JS::Heap<JSObject*> mJsDeviceAddresses;
  nsString mAddress;
  nsString mName;
  bool mDiscoverable;
  bool mDiscovering;
  bool mPairable;
  bool mPowered;
  uint32_t mPairableTimeout;
  uint32_t mDiscoverableTimeout;
  uint32_t mClass;
  nsTArray<nsString> mDeviceAddresses;
  nsTArray<nsString> mUuids;
  bool mIsRooted;
};

END_BLUETOOTH_NAMESPACE

#endif
