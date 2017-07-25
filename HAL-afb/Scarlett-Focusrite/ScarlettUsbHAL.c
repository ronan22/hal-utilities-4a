/*
 * Copyright (C) 2016 "IoT.bzh"
 * Author Fulup Ar Foll <fulup@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * 
 * To find out which control your sound card uses
 *  aplay -l  # Check sndcard name name in between []
 *  amixer -D hw:USB controls # get supported controls
 *  amixer -Dhw:USB cget name=Power-Switch
 *  amixer -Dhw:USB cset name=Power-Switch true
 * 
 */
#define _GNU_SOURCE 
#include "hal-interface.h"
#include "audio-interface.h" 

STATIC struct json_object* MasterOnOff (alsaHalCtlMapT *control, void* handle, struct json_object *valJ) {
    struct json_object *reponseJ;
    
    AFB_INFO ("Power Set value=%s", json_object_get_string(valJ));
    
    reponseJ=json_object_new_object();
    json_object_object_add (reponseJ, "Callback", json_object_new_string("Hello From HAL"));
    
    return reponseJ;
}

// Map HAL hight sndctl with Alsa numid and optionally with a custom callback for non Alsa supported functionalities. 
STATIC alsaHalMapT  alsaHalMap[]= { 
  { .tag=Master_Playback_Volume, .  ctl={.numid=04 } },
  { .tag=PCM_Playback_Volume     , .ctl={.numid=06 } },
  { .tag=PCM_Playback_Switch     , .ctl={.numid=05 } },
  { .tag=Capture_Volume          , .ctl={.numid=12 } },
  { .tag=Master_OnOff_Switch     , .ctl={.numid=0, .type=SND_CTL_ELEM_TYPE_BOOLEAN, .count=1, .name="AGL-Power-Switch"},  .cb={.callback=MasterOnOff, .handle=NULL}},
  { .tag=Master_Playback_Ramp    , .ctl={.numid=0, .type=SND_CTL_ELEM_TYPE_INTEGER, .count=2, .name="AGL-Volume-Switch"}, .cb={.callback=MasterOnOff, .handle=NULL}},
  { .tag=EndHalCrlTag}  /* marker for end of the array */
} ;

// HAL sound card mapping info
STATIC alsaHalSndCardT alsaHalSndCard  = {
    .name  = "Scarlett 18i8 USB", //  WARNING: name MUST match with 'aplay -l'
    .info  = "Hardware Abstraction Layer for Scarlett Focusrite USB professional music sound card",
    .ctls  = alsaHalMap,
};


STATIC int sndServiceInit () {
    int err;
    AFB_DEBUG ("IntelHalBinding Init");
    
    err = halServiceInit (afbBindingV2.api, &alsaHalSndCard);
    return err;
}

// API prefix should be unique for each snd card
PUBLIC const struct afb_binding_v2 afbBindingV2 = {
    .api     = "scarlett-usb",
    .init    = sndServiceInit,
    .verbs   = halServiceApi,
    .onevent = halServiceEvent,
};
