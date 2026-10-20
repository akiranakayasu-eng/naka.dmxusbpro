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
   80,
   80,
   1020,
   860
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
     "maxclass": "comment",
     "patching_rect": [
      20,
      12,
      400,
      34
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "naka.dmxusbpro",
     "fontsize": 24.0
    }
   },
   {
    "box": {
     "id": "obj-2",
     "maxclass": "comment",
     "patching_rect": [
      20,
      46,
      560,
      36
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "Send and receive DMX through the ENTTEC DMX USB Pro. Message-compatible with dmxusbpro 1.x by Olaf Matthes. / ENTTEC DMX USB Pro で DMX を送受信します。"
    }
   },
   {
    "box": {
     "id": "obj-3",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      640,
      140,
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
     "id": "obj-4",
     "maxclass": "message",
     "patching_rect": [
      20,
      100,
      49,
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
     "id": "obj-5",
     "maxclass": "comment",
     "patching_rect": [
      250,
      100,
      360,
      40
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "<channel> <value> pairs, channel 1-512 / チャンネルと値のペア"
    }
   },
   {
    "box": {
     "id": "obj-6",
     "maxclass": "message",
     "patching_rect": [
      20,
      130,
      126,
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
     "id": "obj-7",
     "maxclass": "comment",
     "patching_rect": [
      250,
      130,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "several pairs at once / 複数ペアも可"
    }
   },
   {
    "box": {
     "id": "obj-8",
     "maxclass": "message",
     "patching_rect": [
      20,
      160,
      40,
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
     "id": "obj-9",
     "maxclass": "comment",
     "patching_rect": [
      250,
      160,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "channel 0 sets all channels / ch 0 は全チャンネル"
    }
   },
   {
    "box": {
     "id": "obj-10",
     "maxclass": "message",
     "patching_rect": [
      20,
      190,
      119,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "ch 1 255 128 64"
    }
   },
   {
    "box": {
     "id": "obj-11",
     "maxclass": "comment",
     "patching_rect": [
      250,
      190,
      360,
      40
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "ch <start> v1 v2 ... consecutive channels (extension)"
    }
   },
   {
    "box": {
     "id": "obj-12",
     "maxclass": "message",
     "patching_rect": [
      20,
      220,
      63,
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
     "id": "obj-13",
     "maxclass": "comment",
     "patching_rect": [
      250,
      220,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "print available devices / デバイス一覧を表示"
    }
   },
   {
    "box": {
     "id": "obj-14",
     "maxclass": "message",
     "patching_rect": [
      20,
      250,
      203,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "open /dev/cu.usbserial-XXXX"
    }
   },
   {
    "box": {
     "id": "obj-15",
     "maxclass": "comment",
     "patching_rect": [
      250,
      250,
      360,
      40
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "open a device (COM3 on Windows, or a unique part of the name)"
    }
   },
   {
    "box": {
     "id": "obj-16",
     "maxclass": "message",
     "patching_rect": [
      20,
      280,
      49,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "close"
    }
   },
   {
    "box": {
     "id": "obj-17",
     "maxclass": "comment",
     "patching_rect": [
      250,
      280,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "close the device / 切断"
    }
   },
   {
    "box": {
     "id": "obj-18",
     "maxclass": "message",
     "patching_rect": [
      20,
      310,
      42,
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
     "id": "obj-19",
     "maxclass": "comment",
     "patching_rect": [
      250,
      310,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "output mode (default) / 送信モード"
    }
   },
   {
    "box": {
     "id": "obj-20",
     "maxclass": "message",
     "patching_rect": [
      20,
      340,
      63,
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
     "id": "obj-21",
     "maxclass": "comment",
     "patching_rect": [
      250,
      340,
      360,
      40
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "input mode: left outlet gives <startcode> <channel> <value>"
    }
   },
   {
    "box": {
     "id": "obj-22",
     "maxclass": "message",
     "patching_rect": [
      20,
      370,
      91,
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
     "id": "obj-23",
     "maxclass": "comment",
     "patching_rect": [
      250,
      370,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "DMX start code (0 = dimmer data)"
    }
   },
   {
    "box": {
     "id": "obj-24",
     "maxclass": "message",
     "patching_rect": [
      20,
      400,
      84,
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
     "id": "obj-25",
     "maxclass": "comment",
     "patching_rect": [
      250,
      400,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "max frames per second (1-44)"
    }
   },
   {
    "box": {
     "id": "obj-26",
     "maxclass": "message",
     "patching_rect": [
      20,
      430,
      98,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "channels 512"
    }
   },
   {
    "box": {
     "id": "obj-27",
     "maxclass": "comment",
     "patching_rect": [
      250,
      430,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "number of channels to send (1-512)"
    }
   },
   {
    "box": {
     "id": "obj-28",
     "maxclass": "message",
     "patching_rect": [
      20,
      460,
      77,
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
     "id": "obj-29",
     "maxclass": "comment",
     "patching_rect": [
      250,
      460,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "print firmware, break and MAB time"
    }
   },
   {
    "box": {
     "id": "obj-30",
     "maxclass": "message",
     "patching_rect": [
      20,
      490,
      77,
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
     "id": "obj-31",
     "maxclass": "comment",
     "patching_rect": [
      250,
      490,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "print the widget serial number"
    }
   },
   {
    "box": {
     "id": "obj-32",
     "maxclass": "message",
     "patching_rect": [
      20,
      520,
      49,
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
     "id": "obj-33",
     "maxclass": "comment",
     "patching_rect": [
      250,
      520,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "print state to the Max window"
    }
   },
   {
    "box": {
     "id": "obj-34",
     "maxclass": "message",
     "patching_rect": [
      20,
      550,
      42,
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
     "id": "obj-35",
     "maxclass": "comment",
     "patching_rect": [
      250,
      550,
      360,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "resend the current frame"
    }
   },
   {
    "box": {
     "id": "obj-36",
     "maxclass": "toggle",
     "patching_rect": [
      20,
      580,
      22,
      22
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
     "id": "obj-37",
     "maxclass": "message",
     "patching_rect": [
      50,
      580,
      91,
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
     "id": "obj-38",
     "maxclass": "comment",
     "patching_rect": [
      250,
      580,
      330,
      40
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "temporarily output zeros, values are kept / 一時的に全0"
    }
   },
   {
    "box": {
     "id": "obj-39",
     "maxclass": "comment",
     "patching_rect": [
      640,
      100,
      250,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "DMX timing, advanced / 上級者向け"
    }
   },
   {
    "box": {
     "id": "obj-40",
     "maxclass": "message",
     "patching_rect": [
      640,
      124,
      105,
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
     "id": "obj-41",
     "maxclass": "message",
     "patching_rect": [
      640,
      154,
      84,
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
     "id": "obj-42",
     "maxclass": "comment",
     "patching_rect": [
      760,
      124,
      120,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "microseconds"
    }
   },
   {
    "box": {
     "id": "obj-43",
     "maxclass": "comment",
     "patching_rect": [
      760,
      154,
      120,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "microseconds"
    }
   },
   {
    "box": {
     "id": "obj-44",
     "maxclass": "comment",
     "patching_rect": [
      640,
      200,
      250,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "faders: channel 1-8 / フェーダー"
    }
   },
   {
    "box": {
     "id": "obj-45",
     "maxclass": "slider",
     "patching_rect": [
      640,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-46",
     "maxclass": "message",
     "patching_rect": [
      640,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "1 $1"
    }
   },
   {
    "box": {
     "id": "obj-47",
     "maxclass": "comment",
     "patching_rect": [
      644,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "1"
    }
   },
   {
    "box": {
     "id": "obj-48",
     "maxclass": "slider",
     "patching_rect": [
      674,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-49",
     "maxclass": "message",
     "patching_rect": [
      674,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "2 $1"
    }
   },
   {
    "box": {
     "id": "obj-50",
     "maxclass": "comment",
     "patching_rect": [
      678,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "2"
    }
   },
   {
    "box": {
     "id": "obj-51",
     "maxclass": "slider",
     "patching_rect": [
      708,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-52",
     "maxclass": "message",
     "patching_rect": [
      708,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "3 $1"
    }
   },
   {
    "box": {
     "id": "obj-53",
     "maxclass": "comment",
     "patching_rect": [
      712,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "3"
    }
   },
   {
    "box": {
     "id": "obj-54",
     "maxclass": "slider",
     "patching_rect": [
      742,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-55",
     "maxclass": "message",
     "patching_rect": [
      742,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "4 $1"
    }
   },
   {
    "box": {
     "id": "obj-56",
     "maxclass": "comment",
     "patching_rect": [
      746,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "4"
    }
   },
   {
    "box": {
     "id": "obj-57",
     "maxclass": "slider",
     "patching_rect": [
      776,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-58",
     "maxclass": "message",
     "patching_rect": [
      776,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "5 $1"
    }
   },
   {
    "box": {
     "id": "obj-59",
     "maxclass": "comment",
     "patching_rect": [
      780,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "5"
    }
   },
   {
    "box": {
     "id": "obj-60",
     "maxclass": "slider",
     "patching_rect": [
      810,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-61",
     "maxclass": "message",
     "patching_rect": [
      810,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "6 $1"
    }
   },
   {
    "box": {
     "id": "obj-62",
     "maxclass": "comment",
     "patching_rect": [
      814,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "6"
    }
   },
   {
    "box": {
     "id": "obj-63",
     "maxclass": "slider",
     "patching_rect": [
      844,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-64",
     "maxclass": "message",
     "patching_rect": [
      844,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "7 $1"
    }
   },
   {
    "box": {
     "id": "obj-65",
     "maxclass": "comment",
     "patching_rect": [
      848,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "7"
    }
   },
   {
    "box": {
     "id": "obj-66",
     "maxclass": "slider",
     "patching_rect": [
      878,
      224,
      24,
      120
     ],
     "numinlets": 1,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "size": 256.0
    }
   },
   {
    "box": {
     "id": "obj-67",
     "maxclass": "message",
     "patching_rect": [
      878,
      350,
      32,
      22
     ],
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "text": "8 $1"
    }
   },
   {
    "box": {
     "id": "obj-68",
     "maxclass": "comment",
     "patching_rect": [
      882,
      376,
      24,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "8"
    }
   },
   {
    "box": {
     "id": "obj-69",
     "maxclass": "message",
     "patching_rect": [
      470,
      560,
      42,
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
     "id": "obj-70",
     "maxclass": "umenu",
     "patching_rect": [
      300,
      680,
      200,
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
     "id": "obj-71",
     "maxclass": "comment",
     "patching_rect": [
      510,
      680,
      330,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "choose a device, None closes / デバイス選択"
    }
   },
   {
    "box": {
     "id": "obj-72",
     "maxclass": "comment",
     "patching_rect": [
      530,
      560,
      250,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "fill the menu with devices"
    }
   },
   {
    "box": {
     "id": "obj-73",
     "maxclass": "toggle",
     "patching_rect": [
      170,
      680,
      22,
      22
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
     "id": "obj-74",
     "maxclass": "comment",
     "patching_rect": [
      196,
      680,
      100,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "connected"
    }
   },
   {
    "box": {
     "id": "obj-75",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      720,
      63,
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
     "id": "obj-76",
     "maxclass": "comment",
     "patching_rect": [
      90,
      720,
      160,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "start code 0 only"
    }
   },
   {
    "box": {
     "id": "obj-77",
     "maxclass": "newobj",
     "patching_rect": [
      20,
      750,
      140,
      22
     ],
     "numinlets": 5,
     "numoutlets": 5,
     "outlettype": [
      "",
      "",
      "",
      "",
      ""
     ],
     "text": "route 1 2 3 4"
    }
   },
   {
    "box": {
     "id": "obj-78",
     "maxclass": "number",
     "patching_rect": [
      20,
      780,
      34,
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
     "id": "obj-79",
     "maxclass": "number",
     "patching_rect": [
      56,
      780,
      34,
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
     "id": "obj-80",
     "maxclass": "number",
     "patching_rect": [
      92,
      780,
      34,
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
     "id": "obj-81",
     "maxclass": "number",
     "patching_rect": [
      128,
      780,
      34,
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
     "id": "obj-82",
     "maxclass": "comment",
     "patching_rect": [
      170,
      780,
      220,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "received values ch 1-4 / 受信値"
    }
   },
   {
    "box": {
     "id": "obj-83",
     "maxclass": "newobj",
     "patching_rect": [
      300,
      750,
      98,
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
     "id": "obj-84",
     "maxclass": "number",
     "patching_rect": [
      300,
      780,
      46,
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
     "id": "obj-85",
     "maxclass": "number",
     "patching_rect": [
      350,
      780,
      46,
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
     "id": "obj-86",
     "maxclass": "number",
     "patching_rect": [
      400,
      780,
      46,
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
     "id": "obj-87",
     "maxclass": "comment",
     "patching_rect": [
      300,
      806,
      220,
      20
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "start code / channel / value"
    }
   },
   {
    "box": {
     "id": "obj-88",
     "maxclass": "comment",
     "patching_rect": [
      640,
      420,
      360,
      110
     ],
     "numinlets": 1,
     "numoutlets": 0,
     "text": "Creation arguments: [<device> [<channels> [<refresh>]]]\nAttributes: @channels @refresh @startcode @baudrate (accepted, no effect over USB)\n\nRequires the FTDI virtual COM port driver (macOS: built in)."
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
      "obj-16",
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
      "obj-3",
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
      "obj-3",
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
      "obj-3",
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
      "obj-3",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-26",
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
      "obj-28",
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
      "obj-30",
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
      "obj-32",
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
      "obj-34",
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
      "obj-36",
      0
     ],
     "destination": [
      "obj-37",
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
      "obj-3",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-40",
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
      "obj-41",
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
      "obj-45",
      0
     ],
     "destination": [
      "obj-46",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-46",
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
      "obj-48",
      0
     ],
     "destination": [
      "obj-49",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-49",
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
      "obj-51",
      0
     ],
     "destination": [
      "obj-52",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-52",
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
      "obj-54",
      0
     ],
     "destination": [
      "obj-55",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-55",
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
      "obj-57",
      0
     ],
     "destination": [
      "obj-58",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-58",
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
      "obj-60",
      0
     ],
     "destination": [
      "obj-61",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-61",
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
      "obj-63",
      0
     ],
     "destination": [
      "obj-64",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-64",
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
      "obj-66",
      0
     ],
     "destination": [
      "obj-67",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-67",
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
      "obj-69",
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
      2
     ],
     "destination": [
      "obj-70",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-70",
      1
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
      1
     ],
     "destination": [
      "obj-73",
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
      "obj-75",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-75",
      0
     ],
     "destination": [
      "obj-77",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-77",
      0
     ],
     "destination": [
      "obj-78",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-77",
      1
     ],
     "destination": [
      "obj-79",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-77",
      2
     ],
     "destination": [
      "obj-80",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-77",
      3
     ],
     "destination": [
      "obj-81",
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
      "obj-83",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-83",
      0
     ],
     "destination": [
      "obj-84",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-83",
      1
     ],
     "destination": [
      "obj-85",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "obj-83",
      2
     ],
     "destination": [
      "obj-86",
      0
     ]
    }
   }
  ]
 }
}