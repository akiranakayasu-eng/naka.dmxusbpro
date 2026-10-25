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
  "showrootpatcherontab": 0,
  "showontab": 0,
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
      "showontab": 1,
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
          "naka.artnet"
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
          125,
          460,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "1  set the receiver's IP address (studio ODE Mk2: 10.7.157.222)"
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "message",
         "patching_rect": [
          20,
          150,
          131,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "host 10.7.157.222"
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "message",
         "patching_rect": [
          160,
          150,
          40,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "host"
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "comment",
         "patching_rect": [
          205,
          150,
          170,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "empty host = broadcast"
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "multislider",
         "patching_rect": [
          20,
          190,
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
         "id": "obj-7",
         "maxclass": "comment",
         "patching_rect": [
          350,
          230,
          300,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "2  drag to set channels 1-16; sending starts with the first value"
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          315,
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
         "id": "obj-9",
         "maxclass": "toggle",
         "patching_rect": [
          440,
          315,
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
         "id": "obj-10",
         "maxclass": "message",
         "patching_rect": [
          440,
          345,
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
         "id": "obj-11",
         "maxclass": "comment",
         "patching_rect": [
          470,
          315,
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
         "id": "obj-12",
         "maxclass": "message",
         "patching_rect": [
          20,
          355,
          45,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "start"
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "message",
         "patching_rect": [
          70,
          355,
          40,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "stop"
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "comment",
         "patching_rect": [
          115,
          355,
          300,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "stop hands the lights over (see the 'box' tab)"
        }
       },
       {
        "box": {
         "id": "obj-15",
         "maxclass": "newobj",
         "patching_rect": [
          320,
          395,
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
         "id": "obj-16",
         "maxclass": "comment",
         "patching_rect": [
          400,
          395,
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
         "id": "obj-17",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          395,
          170,
          22
         ],
         "numinlets": 1,
         "numoutlets": 2,
         "outlettype": [
          "",
          ""
         ],
         "text": "naka.artnet 10.7.157.222 0"
        }
       },
       {
        "box": {
         "id": "obj-18",
         "maxclass": "toggle",
         "patching_rect": [
          180,
          435,
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
         "id": "obj-19",
         "maxclass": "comment",
         "patching_rect": [
          210,
          435,
          100,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "sending"
        }
       },
       {
        "box": {
         "id": "obj-20",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          470,
          110,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "print naka.artnet"
        }
       },
       {
        "box": {
         "id": "obj-21",
         "maxclass": "comment",
         "patching_rect": [
          135,
          470,
          200,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "node and iface replies"
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
          "obj-17",
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
          "obj-17",
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
          "obj-8",
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
          "obj-17",
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
          "obj-10",
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
          "obj-12",
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
          "obj-13",
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
          "obj-15",
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
          "obj-17",
          1
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
          "obj-17",
          0
         ],
         "destination": [
          "obj-20",
          0
         ]
        }
       }
      ]
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
     "text": "p network",
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
      "showontab": 1,
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "comment",
         "patching_rect": [
          20,
          15,
          420,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "Find nodes and choose the network card to send from"
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "message",
         "patching_rect": [
          20,
          45,
          75,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "getifaces"
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "comment",
         "patching_rect": [
          110,
          45,
          420,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "list this computer's interfaces (also printed in the Max Console)",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "message",
         "patching_rect": [
          20,
          80,
          124,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "bind 10.7.157.31"
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "message",
         "patching_rect": [
          150,
          80,
          40,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "bind"
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "comment",
         "patching_rect": [
          200,
          80,
          440,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "send from this address only; needed when the computer has Wi-Fi and wired networks",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-7",
         "maxclass": "message",
         "patching_rect": [
          20,
          125,
          40,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "poll"
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "comment",
         "patching_rect": [
          70,
          125,
          520,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "ArtPoll: every Art-Net node answers with 'node <ip> <name> <long name> <universes>'",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-9",
         "maxclass": "message",
         "patching_rect": [
          20,
          160,
          131,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "poll 10.7.157.255"
        }
       },
       {
        "box": {
         "id": "obj-10",
         "maxclass": "comment",
         "patching_rect": [
          150,
          160,
          250,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "poll one subnet or one node",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-11",
         "maxclass": "message",
         "patching_rect": [
          20,
          210,
          117,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "protocol artnet"
        }
       },
       {
        "box": {
         "id": "obj-12",
         "maxclass": "message",
         "patching_rect": [
          130,
          210,
          103,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "protocol sacn"
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "comment",
         "patching_rect": [
          230,
          210,
          360,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "sACN (E1.31): no host = multicast 239.255.<hi>.<lo>",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "message",
         "patching_rect": [
          20,
          245,
          82,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "universe 0"
        }
       },
       {
        "box": {
         "id": "obj-15",
         "maxclass": "message",
         "patching_rect": [
          100,
          245,
          82,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "universe 1"
        }
       },
       {
        "box": {
         "id": "obj-16",
         "maxclass": "comment",
         "patching_rect": [
          180,
          245,
          440,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "Art-Net: 0-32767 = Net*256 + Sub-Net*16 + Universe. sACN: 1-63999",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-17",
         "maxclass": "message",
         "patching_rect": [
          20,
          280,
          96,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "priority 100"
        }
       },
       {
        "box": {
         "id": "obj-18",
         "maxclass": "message",
         "patching_rect": [
          115,
          280,
          110,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "sourcename Max"
        }
       },
       {
        "box": {
         "id": "obj-19",
         "maxclass": "comment",
         "patching_rect": [
          230,
          280,
          100,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "sACN only",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-20",
         "maxclass": "message",
         "patching_rect": [
          20,
          315,
          82,
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
         "id": "obj-21",
         "maxclass": "message",
         "patching_rect": [
          100,
          315,
          110,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "keepalive 1000"
        }
       },
       {
        "box": {
         "id": "obj-22",
         "maxclass": "message",
         "patching_rect": [
          210,
          315,
          89,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "channels 68"
        }
       },
       {
        "box": {
         "id": "obj-23",
         "maxclass": "comment",
         "patching_rect": [
          300,
          315,
          330,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "keepalive: repeat an unchanged frame (ms, 0 = off)",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-24",
         "maxclass": "message",
         "patching_rect": [
          20,
          350,
          47,
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
         "id": "obj-25",
         "maxclass": "comment",
         "patching_rect": [
          65,
          350,
          150,
          24
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "current settings",
         "bubble": 1,
         "bubbleside": 1
        }
       },
       {
        "box": {
         "id": "obj-26",
         "maxclass": "newobj",
         "patching_rect": [
          20,
          400,
          70,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
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
          "obj-26",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-22",
          0
         ],
         "destination": [
          "obj-26",
          0
         ]
        }
       },
       {
        "patchline": {
         "source": [
          "obj-24",
          0
         ],
         "destination": [
          "obj-26",
          0
         ]
        }
       }
      ]
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
     "text": "p box",
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
      "showontab": 1,
      "boxes": [
       {
        "box": {
         "id": "obj-1",
         "maxclass": "comment",
         "patching_rect": [
          20,
          15,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "Receivers that accept one sender at a time"
        }
       },
       {
        "box": {
         "id": "obj-2",
         "maxclass": "comment",
         "patching_rect": [
          20,
          37,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "The self-built Art-Net box (miseru) locks onto the first sender's IP address."
        }
       },
       {
        "box": {
         "id": "obj-3",
         "maxclass": "comment",
         "patching_rect": [
          20,
          59,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "It keeps the lock while frames keep coming and lets go 5 s after they stop,"
        }
       },
       {
        "box": {
         "id": "obj-4",
         "maxclass": "comment",
         "patching_rect": [
          20,
          81,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "then outputs zeros until the next sender. naka.artnet repeats the last frame"
        }
       },
       {
        "box": {
         "id": "obj-5",
         "maxclass": "comment",
         "patching_rect": [
          20,
          103,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "every @keepalive ms (default 1000), so an open patch keeps the lock."
        }
       },
       {
        "box": {
         "id": "obj-6",
         "maxclass": "comment",
         "patching_rect": [
          20,
          125,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": ""
        }
       },
       {
        "box": {
         "id": "obj-7",
         "maxclass": "comment",
         "patching_rect": [
          20,
          147,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "- send 'stop' to hand the lights over; values are kept and 'start' resumes"
        }
       },
       {
        "box": {
         "id": "obj-8",
         "maxclass": "comment",
         "patching_rect": [
          20,
          169,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "- after 'stop', moving a slider does not restart sending, only 'start' does"
        }
       },
       {
        "box": {
         "id": "obj-9",
         "maxclass": "comment",
         "patching_rect": [
          20,
          191,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "- use @bind so the box always sees the same address from this computer"
        }
       },
       {
        "box": {
         "id": "obj-10",
         "maxclass": "comment",
         "patching_rect": [
          20,
          213,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "- an ENTTEC ODE Mk2 merges up to two senders instead; avoid sending"
        }
       },
       {
        "box": {
         "id": "obj-11",
         "maxclass": "comment",
         "patching_rect": [
          20,
          235,
          640,
          20
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "text": "  from Max while the studio iPad (Luminair) is also sending"
        }
       },
       {
        "box": {
         "id": "obj-12",
         "maxclass": "message",
         "patching_rect": [
          20,
          280,
          40,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "stop"
        }
       },
       {
        "box": {
         "id": "obj-13",
         "maxclass": "message",
         "patching_rect": [
          70,
          280,
          45,
          22
         ],
         "numinlets": 2,
         "numoutlets": 1,
         "outlettype": [
          ""
         ],
         "text": "start"
        }
       },
       {
        "box": {
         "id": "obj-14",
         "maxclass": "message",
         "patching_rect": [
          125,
          280,
          45,
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
         "maxclass": "newobj",
         "patching_rect": [
          20,
          320,
          70,
          22
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "outlettype": [],
         "text": "s ---naka"
        }
       }
      ],
      "lines": [
       {
        "patchline": {
         "source": [
          "obj-12",
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
          "obj-13",
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
          "obj-14",
          0
         ],
         "destination": [
          "obj-15",
          0
         ]
        }
       }
      ]
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
      "showontab": 1,
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
          "naka.artnet"
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
          640,
          300
         ],
         "numinlets": 1,
         "numoutlets": 0,
         "linecount": 16,
         "text": "Creation arguments\n  [naka.artnet <host> <universe>]   both optional, attributes may follow\n  <host> is the receiver's IP address; without it Art-Net is broadcast and sACN is multicast.\n\nSame messages as naka.dmxusbpro\n  <channel> <value> pairs, ch <start> v1 v2 ..., blackout, bang. One object sends one universe;\n  use several objects for several universes.\n\nNetwork\n  Art-Net 4 ArtDmx to UDP port 6454, ArtPoll discovery. sACN (ANSI E1.31) to UDP port 5568,\n  stream-terminated packets on stop. Sending runs on a background thread.\n\nOpen source (MIT): github.com/akiranakayasu-eng/naka.dmxusbpro"
        }
       }
      ],
      "lines": []
     }
    }
   }
  ],
  "lines": []
 }
}