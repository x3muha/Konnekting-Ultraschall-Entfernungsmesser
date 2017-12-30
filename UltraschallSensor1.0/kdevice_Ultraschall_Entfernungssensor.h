#define MANUFACTURER_ID 1337
#define DEVICE_ID 1
#define REVISION 1

#define COMOBJ_Entfernung 0
#define PARAM_Anlaufzeit 0
#define PARAM_on_off 1
#define PARAM_Zyklus 2
#define PARAM_SBC 3
#define PARAM_delay 4
        
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - Entfernung */ KnxComObject(KNX_DPT_7_011, 0x34)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - Anlaufzeit */ PARAM_UINT8,
    /* Index 1 - on_off */ PARAM_UINT8,
    /* Index 2 - Zyklus */ PARAM_UINT16,
    /* Index 3 - SBC */ PARAM_UINT8,
    /* Index 4 - delay */ PARAM_UINT16
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code
