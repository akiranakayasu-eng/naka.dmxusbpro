{
 "patcher": {
  "fileversion": 1,
  "appversion": {
   "major": 8,
   "minor": 6,
   "revision": 0,
   "architecture": "x64",
   "modernui": 1
  },
  "classnamespace": "box",
  "rect": [
   100,
   100,
   760,
   680
  ],
  "default_fontsize": 12.0,
  "default_fontname": "Arial",
  "gridsize": [
   15.0,
   15.0
  ],
  "boxes": [
   {
    "box": {
     "id": "obj-1",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      20,
      90,
      22
     ],
     "numinlets": 0,
     "numoutlets": 0,
     "outlettype": [],
     "text": "p basic",
     "patcher": {
      "fileversion": 1,
      "appversion": {
       "major": 8,
       "minor": 6,
       "revision": 0,
       "architecture": "x64",
       "modernui": 1
      },
      "classnamespace": "box",
      "rect": [
       0,
       26,
       760,
       800
      ],
      "default_fontsize": 12.0,
      "default_fontname": "Arial",
      "gridsize": [
       15.0,
       15.0
      ],
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "jsui",
         "patching_rect": [
          10,
          10,
          640,
          100
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "filename": "helpdetails.js",
         "jsarguments": [
          "naka.dmxusbpro"
         ],
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          130,
          72,
          22
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          "bang"
         ],
         "text": "loadbang"
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "message",
         "patching_rect": [
          20,
          160,
          44,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "menu"
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "comment",
         "patching_rect": [
          70,
          160,
          180,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "refresh the device list",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          440,
          130,
          22
         ],
         "numinlets": 1,
         "numoutlets": 3,
         "outlettype": [
          "",
          "int",
          ""
         ],
         "text": "naka.dmxusbpro"
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "umenu",
         "patching_rect": [
          190,
          200,
          240,
          22
         ],
         "numinlets": 1,
         "numoutlets": 3,
         "outlettype": [
          "int",
          "",
          ""
         ],
         "items": "None"
        }
       },
       {
        "box": {
         "id": "obj-7",
         "maxclass": "comment",
         "patching_rect": [
          440,
          200,
          230,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "1  choose your DMX USB Pro",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "toggle",
         "patching_rect": [
          190,
          480,
          24,
          24
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          "int"
         ]
        }
       },
       {
        "box": {
         "id": "obj-9",
         "maxclass": "comment",
         "patching_rect": [
          220,
          480,
          110,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "connected",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-10",
         "maxclass": "multislider",
         "patching_rect": [
          20,
          250,
          320,
          110
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "size": 16,
         "setminmax": [
          0.0,
          255.0
         ],
         "setstyle": 1,
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-11",
         "maxclass": "comment",
         "patching_rect": [
          350,
          290,
          230,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "2  drag to set channels 1-16",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-12",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          380,
          100,
          22
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "listfunnel 1"
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "comment",
         "patching_rect": [
          110,
          380,
          300,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "turns the list into <channel> <value> pairs"
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "toggle",
         "patching_rect": [
          440,
          380,
          24,
          24
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          "int"
         ]
        }
       },
       {
        "box": {
         "id": "obj-15",
         "maxclass": "message",
         "patching_rect": [
          440,
          410,
          93,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "blackout $1"
        }
       },
       {
        "box": {
         "id": "obj-16",
         "maxclass": "comment",
         "patching_rect": [
          470,
          380,
          200,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "blackout (values are kept)"
        }
       },
       {
        "box": {
         "id": "obj-17",
         "maxclass": "newobj",
         "patching_rect": [
          320,
          440,
          79,
          22
         ],
         "numinlets": 0,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "r ---naka"
        }
       },
       {
        "box": {
         "id": "obj-18",
         "maxclass": "comment",
         "patching_rect": [
          400,
          440,
          220,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "messages from the other tabs"
        }
       },
       {
        "box": {
         "id": "obj-19",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          520,
          100,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka.in"
        }
       },
       {
        "box": {
         "id": "obj-20",
         "maxclass": "comment",
         "patching_rect": [
          120,
          520,
          280,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "received DMX goes to the 'receive' tab"
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "obj-2",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-3",
          0
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-5",
          2
         ],
         "destination": [
          "obj-6",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-6",
          1
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-5",
          1
         ],
         "destination": [
          "obj-8",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-10",
          0
         ],
         "destination": [
          "obj-12",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-12",
          0
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-14",
          0
         ],
         "destination": [
          "obj-15",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-15",
          0
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-17",
          0
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-5",
          0
         ],
         "destination": [
          "obj-19",
          0
         ]
        }
       }
      ],
      "showontab": 1
     },
     "saved_object_attributes": {
      "description": "",
      "digest": "",
      "globalpatchername": "",
      "tags": ""
     }
    }
   },
   {
    "box": {
     "id": "obj-2",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      50,
      90,
      22
     ],
     "numinlets": 0,
     "numoutlets": 0,
     "outlettype": [],
     "text": "p messages",
     "patcher": {
      "fileversion": 1,
      "appversion": {
       "major": 8,
       "minor": 6,
       "revision": 0,
       "architecture": "x64",
       "modernui": 1
      },
      "classnamespace": "box",
      "rect": [
       0,
       26,
       760,
       800
      ],
      "default_fontsize": 12.0,
      "default_fontname": "Arial",
      "gridsize": [
       15.0,
       15.0
      ],
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "jsui",
         "patching_rect": [
          10,
          10,
          640,
          100
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "filename": "helpdetails.js",
         "jsarguments": [
          "naka.dmxusbpro"
         ],
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          320,
          79,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "message",
         "patching_rect": [
          20,
          130,
          51,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "1 255"
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "comment",
         "patching_rect": [
          200,
          130,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "<channel> <value> pairs, channel 1-512, value 0-255"
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "message",
         "patching_rect": [
          20,
          160,
          128,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "1 255 2 128 3 64"
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "comment",
         "patching_rect": [
          200,
          160,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "several pairs in one list"
        }
       },
       {
        "box": {
         "id": "obj-7",
         "maxclass": "message",
         "patching_rect": [
          20,
          190,
          51,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "0 255"
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "comment",
         "patching_rect": [
          200,
          190,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "channel 0 sets every channel"
        }
       },
       {
        "box": {
         "id": "obj-9",
         "maxclass": "message",
         "patching_rect": [
          20,
          220,
          37,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "0 0"
        }
       },
       {
        "box": {
         "id": "obj-10",
         "maxclass": "comment",
         "patching_rect": [
          200,
          220,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "all channels off"
        }
       },
       {
        "box": {
         "id": "obj-11",
         "maxclass": "message",
         "patching_rect": [
          20,
          250,
          128,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "ch 10 255 128 64"
        }
       },
       {
        "box": {
         "id": "obj-12",
         "maxclass": "comment",
         "patching_rect": [
          200,
          250,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "ch <start> v1 v2 ...  consecutive channels"
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "message",
         "patching_rect": [
          20,
          280,
          44,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "bang"
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "comment",
         "patching_rect": [
          200,
          280,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "resend the current frame"
        }
       },
       {
        "box": {
         "id": "obj-15",
         "maxclass": "comment",
         "patching_rect": [
          20,
          360,
          200,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "attributes",
         "fontface": 1
        }
       },
       {
        "box": {
         "id": "obj-16",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          486,
          79,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       },
       {
        "box": {
         "id": "obj-17",
         "maxclass": "message",
         "patching_rect": [
          20,
          386,
          93,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "channels 24"
        }
       },
       {
        "box": {
         "id": "obj-18",
         "maxclass": "comment",
         "patching_rect": [
          200,
          386,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "how many channels to send (1-512)"
        }
       },
       {
        "box": {
         "id": "obj-19",
         "maxclass": "message",
         "patching_rect": [
          20,
          416,
          86,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "refresh 40"
        }
       },
       {
        "box": {
         "id": "obj-20",
         "maxclass": "comment",
         "patching_rect": [
          200,
          416,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "max frames per second (1-44)"
        }
       },
       {
        "box": {
         "id": "obj-21",
         "maxclass": "message",
         "patching_rect": [
          20,
          446,
          93,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "startcode 0"
        }
       },
       {
        "box": {
         "id": "obj-22",
         "maxclass": "comment",
         "patching_rect": [
          200,
          446,
          380,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "DMX start code, 0 = dimmer data"
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "obj-3",
          0
         ],
         "destination": [
          "obj-2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-5",
          0
         ],
         "destination": [
          "obj-2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-7",
          0
         ],
         "destination": [
          "obj-2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-9",
          0
         ],
         "destination": [
          "obj-2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-11",
          0
         ],
         "destination": [
          "obj-2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-13",
          0
         ],
         "destination": [
          "obj-2",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-17",
          0
         ],
         "destination": [
          "obj-16",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-19",
          0
         ],
         "destination": [
          "obj-16",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-21",
          0
         ],
         "destination": [
          "obj-16",
          0
         ]
        }
       }
      ],
      "showontab": 1
     },
     "saved_object_attributes": {
      "description": "",
      "digest": "",
      "globalpatchername": "",
      "tags": ""
     }
    }
   },
   {
    "box": {
     "id": "obj-3",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      80,
      90,
      22
     ],
     "numinlets": 0,
     "numoutlets": 0,
     "outlettype": [],
     "text": "p receive",
     "patcher": {
      "fileversion": 1,
      "appversion": {
       "major": 8,
       "minor": 6,
       "revision": 0,
       "architecture": "x64",
       "modernui": 1
      },
      "classnamespace": "box",
      "rect": [
       0,
       26,
       760,
       800
      ],
      "default_fontsize": 12.0,
      "default_fontname": "Arial",
      "gridsize": [
       15.0,
       15.0
      ],
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "jsui",
         "patching_rect": [
          10,
          10,
          640,
          100
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "filename": "helpdetails.js",
         "jsarguments": [
          "naka.dmxusbpro"
         ],
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "comment",
         "patching_rect": [
          20,
          120,
          300,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "1  switch the interface to input mode",
         "bubble": 1,
         "bubbleside": 2
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "message",
         "patching_rect": [
          20,
          160,
          65,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "receive"
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "message",
         "patching_rect": [
          90,
          160,
          44,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "send"
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          200,
          79,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "comment",
         "patching_rect": [
          160,
          160,
          220,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "send = back to output mode"
        }
       },
       {
        "box": {
         "id": "obj-7",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          260,
          100,
          22
         ],
         "numinlets": 0,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "r ---naka.in"
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "comment",
         "patching_rect": [
          130,
          260,
          420,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "2  each changed slot arrives as <startcode> <channel> <value>",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-9",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          300,
          100,
          22
         ],
         "numinlets": 1,
         "numoutlets": 3,
         "outlettype": [
          "int",
          "int",
          "int"
         ],
         "text": "unpack 0 0 0"
        }
       },
       {
        "box": {
         "id": "obj-10",
         "maxclass": "number",
         "patching_rect": [
          20,
          330,
          60,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-11",
         "maxclass": "comment",
         "patching_rect": [
          20,
          354,
          70,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "start code"
        }
       },
       {
        "box": {
         "id": "obj-12",
         "maxclass": "number",
         "patching_rect": [
          90,
          330,
          60,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "comment",
         "patching_rect": [
          90,
          354,
          70,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "channel"
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "number",
         "patching_rect": [
          160,
          330,
          60,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-15",
         "maxclass": "comment",
         "patching_rect": [
          160,
          354,
          70,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "value"
        }
       },
       {
        "box": {
         "id": "obj-16",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          400,
          65,
          22
         ],
         "numinlets": 2,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "text": "route 0"
        }
       },
       {
        "box": {
         "id": "obj-17",
         "maxclass": "comment",
         "patching_rect": [
          90,
          400,
          220,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "only start code 0 (dimmer data)"
        }
       },
       {
        "box": {
         "id": "obj-18",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          430,
          200,
          22
         ],
         "numinlets": 9,
         "numoutlets": 9,
         "outlettype": [
          "",
          "",
          "",
          "",
          "",
          "",
          "",
          "",
          ""
         ],
         "text": "route 1 2 3 4 5 6 7 8"
        }
       },
       {
        "box": {
         "id": "obj-19",
         "maxclass": "number",
         "patching_rect": [
          20,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-20",
         "maxclass": "comment",
         "patching_rect": [
          32,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "1"
        }
       },
       {
        "box": {
         "id": "obj-21",
         "maxclass": "number",
         "patching_rect": [
          64,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-22",
         "maxclass": "comment",
         "patching_rect": [
          76,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "2"
        }
       },
       {
        "box": {
         "id": "obj-23",
         "maxclass": "number",
         "patching_rect": [
          108,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-24",
         "maxclass": "comment",
         "patching_rect": [
          120,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "3"
        }
       },
       {
        "box": {
         "id": "obj-25",
         "maxclass": "number",
         "patching_rect": [
          152,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-26",
         "maxclass": "comment",
         "patching_rect": [
          164,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "4"
        }
       },
       {
        "box": {
         "id": "obj-27",
         "maxclass": "number",
         "patching_rect": [
          196,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-28",
         "maxclass": "comment",
         "patching_rect": [
          208,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "5"
        }
       },
       {
        "box": {
         "id": "obj-29",
         "maxclass": "number",
         "patching_rect": [
          240,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-30",
         "maxclass": "comment",
         "patching_rect": [
          252,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "6"
        }
       },
       {
        "box": {
         "id": "obj-31",
         "maxclass": "number",
         "patching_rect": [
          284,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-32",
         "maxclass": "comment",
         "patching_rect": [
          296,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "7"
        }
       },
       {
        "box": {
         "id": "obj-33",
         "maxclass": "number",
         "patching_rect": [
          328,
          460,
          40,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          "bang"
         ]
        }
       },
       {
        "box": {
         "id": "obj-34",
         "maxclass": "comment",
         "patching_rect": [
          340,
          484,
          30,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "8"
        }
       },
       {
        "box": {
         "id": "obj-35",
         "maxclass": "comment",
         "patching_rect": [
          380,
          460,
          120,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "channels 1-8"
        }
       },
       {
        "box": {
         "id": "obj-36",
         "maxclass": "comment",
         "patching_rect": [
          20,
          520,
          300,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "whole universe at once",
         "fontface": 1
        }
       },
       {
        "box": {
         "id": "obj-37",
         "maxclass": "message",
         "patching_rect": [
          20,
          546,
          72,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "rxmode 1"
        }
       },
       {
        "box": {
         "id": "obj-38",
         "maxclass": "message",
         "patching_rect": [
          100,
          546,
          72,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "rxmode 0"
        }
       },
       {
        "box": {
         "id": "obj-39",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          576,
          79,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       },
       {
        "box": {
         "id": "obj-40",
         "maxclass": "comment",
         "patching_rect": [
          180,
          546,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "1 = one 512-value list per frame, 0 = changed slots only (default)"
        }
       },
       {
        "box": {
         "id": "obj-41",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          610,
          100,
          22
         ],
         "numinlets": 0,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "r ---naka.in"
        }
       },
       {
        "box": {
         "id": "obj-42",
         "maxclass": "newobj",
         "patching_rect": [
          110,
          610,
          51,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "text": "t l l"
        }
       },
       {
        "box": {
         "id": "obj-43",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          640,
          58,
          22
         ],
         "numinlets": 2,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "text": "zl.len"
        }
       },
       {
        "box": {
         "id": "obj-44",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          670,
          79,
          22
         ],
         "numinlets": 2,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "text": "route 512"
        }
       },
       {
        "box": {
         "id": "obj-45",
         "maxclass": "comment",
         "patching_rect": [
          170,
          610,
          300,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "only 512-value lists go to the display"
        }
       },
       {
        "box": {
         "id": "obj-46",
         "maxclass": "multislider",
         "patching_rect": [
          200,
          680,
          400,
          80
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "size": 512,
         "setminmax": [
          0.0,
          255.0
         ],
         "setstyle": 1,
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-47",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          700,
          58,
          22
         ],
         "numinlets": 2,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "text": "zl.reg"
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "obj-3",
          0
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-4",
          0
         ],
         "destination": [
          "obj-5",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-7",
          0
         ],
         "destination": [
          "obj-9",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-9",
          0
         ],
         "destination": [
          "obj-10",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-9",
          1
         ],
         "destination": [
          "obj-12",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-9",
          2
         ],
         "destination": [
          "obj-14",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-7",
          0
         ],
         "destination": [
          "obj-16",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-16",
          0
         ],
         "destination": [
          "obj-18",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          0
         ],
         "destination": [
          "obj-19",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          1
         ],
         "destination": [
          "obj-21",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          2
         ],
         "destination": [
          "obj-23",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          3
         ],
         "destination": [
          "obj-25",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          4
         ],
         "destination": [
          "obj-27",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          5
         ],
         "destination": [
          "obj-29",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          6
         ],
         "destination": [
          "obj-31",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          7
         ],
         "destination": [
          "obj-33",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-37",
          0
         ],
         "destination": [
          "obj-39",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-38",
          0
         ],
         "destination": [
          "obj-39",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-41",
          0
         ],
         "destination": [
          "obj-42",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-42",
          0
         ],
         "destination": [
          "obj-43",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-43",
          0
         ],
         "destination": [
          "obj-44",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-42",
          1
         ],
         "destination": [
          "obj-47",
          1
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-44",
          0
         ],
         "destination": [
          "obj-47",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-47",
          0
         ],
         "destination": [
          "obj-46",
          0
         ]
        }
       }
      ],
      "showontab": 1
     },
     "saved_object_attributes": {
      "description": "",
      "digest": "",
      "globalpatchername": "",
      "tags": ""
     }
    }
   },
   {
    "box": {
     "id": "obj-4",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      110,
      90,
      22
     ],
     "numinlets": 0,
     "numoutlets": 0,
     "outlettype": [],
     "text": "p interface",
     "patcher": {
      "fileversion": 1,
      "appversion": {
       "major": 8,
       "minor": 6,
       "revision": 0,
       "architecture": "x64",
       "modernui": 1
      },
      "classnamespace": "box",
      "rect": [
       0,
       26,
       760,
       800
      ],
      "default_fontsize": 12.0,
      "default_fontname": "Arial",
      "gridsize": [
       15.0,
       15.0
      ],
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "jsui",
         "patching_rect": [
          10,
          10,
          640,
          100
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "filename": "helpdetails.js",
         "jsarguments": [
          "naka.dmxusbpro"
         ],
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "comment",
         "patching_rect": [
          20,
          120,
          400,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "Results are printed to the Max Console."
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          340,
          79,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "message",
         "patching_rect": [
          20,
          150,
          65,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "getdevs"
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "comment",
         "patching_rect": [
          200,
          150,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "list connected interfaces"
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "message",
         "patching_rect": [
          20,
          180,
          149,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "open serial 2150128"
        }
       },
       {
        "box": {
         "id": "obj-7",
         "maxclass": "comment",
         "patching_rect": [
          200,
          180,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "open the interface with this serial number (see getserial)"
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "message",
         "patching_rect": [
          20,
          210,
          121,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "autoreconnect 1"
        }
       },
       {
        "box": {
         "id": "obj-9",
         "maxclass": "comment",
         "patching_rect": [
          200,
          210,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "reopen automatically when the cable is plugged back in (default on)"
        }
       },
       {
        "box": {
         "id": "obj-10",
         "maxclass": "message",
         "patching_rect": [
          20,
          240,
          79,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "getparams"
        }
       },
       {
        "box": {
         "id": "obj-11",
         "maxclass": "comment",
         "patching_rect": [
          200,
          240,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "firmware version, break / mark-after-break time, refresh rate"
        }
       },
       {
        "box": {
         "id": "obj-12",
         "maxclass": "message",
         "patching_rect": [
          20,
          270,
          79,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "getserial"
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "comment",
         "patching_rect": [
          200,
          270,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "serial number of the interface"
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "message",
         "patching_rect": [
          20,
          300,
          51,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "print"
        }
       },
       {
        "box": {
         "id": "obj-15",
         "maxclass": "comment",
         "patching_rect": [
          200,
          300,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "current state of the object"
        }
       },
       {
        "box": {
         "id": "obj-16",
         "maxclass": "comment",
         "patching_rect": [
          20,
          380,
          460,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "DMX timing (advanced). Most fixtures work with the defaults.",
         "fontface": 1
        }
       },
       {
        "box": {
         "id": "obj-17",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          476,
          79,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       },
       {
        "box": {
         "id": "obj-18",
         "maxclass": "message",
         "patching_rect": [
          20,
          406,
          107,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "breaktime 192"
        }
       },
       {
        "box": {
         "id": "obj-19",
         "maxclass": "comment",
         "patching_rect": [
          200,
          406,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "break length in microseconds (96-1355)"
        }
       },
       {
        "box": {
         "id": "obj-20",
         "maxclass": "message",
         "patching_rect": [
          20,
          436,
          86,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "mabtime 20"
        }
       },
       {
        "box": {
         "id": "obj-21",
         "maxclass": "comment",
         "patching_rect": [
          200,
          436,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "mark-after-break in microseconds (11-1355)"
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "obj-4",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-6",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-8",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-10",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-12",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-14",
          0
         ],
         "destination": [
          "obj-3",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-18",
          0
         ],
         "destination": [
          "obj-17",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-20",
          0
         ],
         "destination": [
          "obj-17",
          0
         ]
        }
       }
      ],
      "showontab": 1
     },
     "saved_object_attributes": {
      "description": "",
      "digest": "",
      "globalpatchername": "",
      "tags": ""
     }
    }
   },
   {
    "box": {
     "id": "obj-5",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      140,
      90,
      22
     ],
     "numinlets": 0,
     "numoutlets": 0,
     "outlettype": [],
     "text": "p ?",
     "patcher": {
      "fileversion": 1,
      "appversion": {
       "major": 8,
       "minor": 6,
       "revision": 0,
       "architecture": "x64",
       "modernui": 1
      },
      "classnamespace": "box",
      "rect": [
       0,
       26,
       760,
       800
      ],
      "default_fontsize": 12.0,
      "default_fontname": "Arial",
      "gridsize": [
       15.0,
       15.0
      ],
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "jsui",
         "patching_rect": [
          10,
          10,
          640,
          100
         ],
         "numinlets": 1,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "filename": "helpdetails.js",
         "jsarguments": [
          "naka.dmxusbpro"
         ],
         "parameter_enable": 0
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "comment",
         "patching_rect": [
          20,
          130,
          700,
          300
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "Creation arguments\n  [naka.dmxusbpro <device> <channels> <refresh>]   all optional\n  <device> is a port name (/dev/cu.usbserial-XXXX, COM3, or a unique part such as EN123456)\n  or a number (Windows: COM<n>, macOS: n-th entry of getdevs).\n\nCompatibility\n  Uses the same messages, attributes and outlets as dmxusbpro 1.x by Olaf Matthes.\n  Existing patches only need the object name changed to naka.dmxusbpro.\n  @baudrate is accepted but has no effect (the interface is a USB device).\n  Firmware update is not supported; use ENTTEC's own tool.\n\nDriver\n  Uses the FTDI virtual COM port. macOS has it built in; Windows installs it automatically.\n\nOpen source (MIT): github.com/akiranakayasu-eng/naka.dmxusbpro"
        }
       }
      ],
      "lines": [],
      "showontab": 1
     },
     "saved_object_attributes": {
      "description": "",
      "digest": "",
      "globalpatchername": "",
      "tags": ""
     }
    }
   }
  ],
  "lines": [],
  "showrootpatcherontab": 0,
  "showontab": 0
 }
}