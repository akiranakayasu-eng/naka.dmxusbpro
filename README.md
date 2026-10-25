# naka.dmxusbpro for Max

ENTTEC DMX USB Pro を Cycling '74 Max から直接動かすためのオープンソース外部オブジェクトです。Art-Net / sACN でネットワークに送る [`naka.artnet`](#nakaartnetart-net--sacn-送信) も同梱しています。
An open-source Max external for the ENTTEC DMX USB Pro (macOS Apple Silicon / Intel, Windows), plus `naka.artnet` for Art-Net / sACN output.

> Status: 開発初期（プロトコル部は単体テスト済み、実機・Max上での動作は未確認）

## 使い方 (Usage)

旧 dmxusbpro 1.x（Olaf Matthes 作、更新終了）とメッセージ互換です。旧オブジェクトと同時にインストールできるよう名前は `naka.dmxusbpro` にしています。既存パッチはオブジェクト名を `dmxusbpro` → `naka.dmxusbpro` に書き換えるだけで動く想定です。

```
[naka.dmxusbpro]                     ← 引数なし。open で接続
[naka.dmxusbpro COM3 512 40]         ← <device> [<channels> <refresh>]
[naka.dmxusbpro @refresh 40 @channels 24]
```

`<device>` はポート名（`/dev/cu.usbserial-XXXX`、`COM3`、または一意な一部分 `EN123456` など）か番号（Windows は `COM<n>`、macOS は `getdevs` の n 番目）。

| 入力メッセージ | 内容 |
|---|---|
| `<ch> <val> [<ch> <val> ...]` | チャンネル (1–512) に値 (0–255、float 可) をセット。ch 0 は全チャンネル |
| `open <device>` / `close` | 接続 / 切断 |
| `open serial <番号>` | シリアル番号で接続（2台使うときにポート名が入れ替わっても安全。新規） |
| `menu` | 右アウトレットに umenu 用の `clear` / `append None` / `append <device>...` を出力 |
| `<device>` / `None` | umenu で選んだ名前をそのまま送ると接続、`None` で切断 |
| `getdevs` | 接続中のデバイス一覧を Max ウィンドウに表示 |
| `blackout 1` / `blackout 0` | 一時的に全チャンネル 0 / 元に戻す（値は保持） |
| `send` / `receive` | 送信モード（既定）/ 受信モード |
| `getparams` / `getserial` / `print` | ファームウェア版・タイミング / シリアル番号 / 状態を Max ウィンドウへ |
| `breaktime <µs>` / `mabtime <µs>` | DMX の Break / Mark After Break 時間 |
| `ch <start> v1 v2 ...` | 連続チャンネルを一括セット（新規拡張） |
| `setparams <break> <mab> <rate>` | ウィジェットのタイミングを生値で設定（新規拡張） |
| `bang` | 現在のフレームを再送（新規拡張） |
| `firmware` | 非対応（ENTTEC 純正ツールを使ってください） |

| 属性 | 既定値 | 内容 |
|---|---|---|
| `@channels` | 512 | 送信チャンネル数 (1–512) |
| `@refresh` | 40 | 送信レート (1–44 fps) |
| `@startcode` | 0 | DMX スタートコード |
| `@baudrate` | 57600 | 互換のため受け付けるだけ（USB 接続なので効果なし） |
| `@rxmode` | 0 | 受信の出力形式。0 = 変化したスロットごと（旧互換）、1 = 毎フレーム 512 個のリスト（新規） |
| `@autoreconnect` | 1 | USB を挿し直したら自動で再接続（新規） |

| アウトレット | 内容 |
|---|---|
| 左 | 受信モード時、変化したスロットごとに `<startcode> <channel> <value>`（`@rxmode 1` なら 512 個のリスト） |
| 中 | 接続状態 (0/1) |
| 右 | umenu 用メッセージ |

ウィジェット自体が最後のフレームを DMX ラインに出し続けるため、値が変わったときだけ（最大 `@refresh` 回/秒）送信します。シリアル I/O はすべてバックグラウンドスレッドで行い、Max のスケジューラを止めません。

## 設計方針 (Design)

- **C 外部オブジェクト (Max SDK)**: `[serial]` + JS でも実装できますが、512ch を毎フレームリストで流すと重く、切断検出や受信パースも扱いにくいため C を選びました。
- **FTDI VCP（仮想シリアルポート）を使用、D2XX は使わない**: macOS は OS 標準ドライバで `/dev/cu.usbserial-XXXX` が出るため追加インストール不要。Windows も FTDI VCP ドライバ（通常自動インストール）で `COMx` になります。D2XX はプロプライエタリなバイナリ同梱が必要になり、OSS 配布と Apple Silicon 対応が面倒になるので見送りました。
- **プロトコル部は OS/Max 非依存** (`source/core/dmxusbpro_protocol.c`)。ハードなしで単体テストできます。
- 対応ラベル: 3 Get Params / 4 Set Params / 5 Received DMX / 6 Output Only Send DMX / 8 Receive DMX on Change / 9 Change of State / 10 Get Serial。
- DMX USB Pro Mk2 の2ポート目は ENTTEC の API キーが必要なため現状対象外です。

## 互換機 (Compatible interfaces)

ENTTEC の API（label 3/4/5/6/8/10）に沿った互換機でも使えます。自作の互換機 dmx_usb_pro（ATmega328P + FT245RL）で確認した注意点:

- シリアル番号を書き込んでいないと `00000001` を返します。2 台を `open serial` で区別するには、それぞれ別の番号を書き込んでください。同じ番号の機器が複数あると、最初に見つかった方を開いて警告を出します。
- label 9（変化分）が無く、変化したフレームを label 5 で返します。オブジェクトはどちらでも同じ出力になるので、`receive` はそのまま使えます。
- `breaktime` / `mabtime` で変えた値は電源を切ると戻ります（純正も同じ）。

## naka.artnet（Art-Net / sACN 送信）

同じリポジトリに、ネットワーク越しに DMX を送る `naka.artnet` も入っています。ENTTEC ODE などの Ethernet-DMX ノードや、miseru 用の自作の箱に、Max から直接 Art-Net 4 または sACN（ANSI E1.31）を送ります。値の送り方は `naka.dmxusbpro` と同じなので、オブジェクト名を書き換えるだけで USB 出力とネットワーク出力を切り替えられます。1 つのオブジェクトが 1 ユニバースを送ります。

```
[naka.artnet 10.7.157.222 0]                 ← <host> <universe>。スタジオの ODE Mk2 の例
[naka.artnet @protocol sacn @universe 1]     ← sACN をマルチキャストで
[naka.artnet 10.7.157.222 @bind 10.7.157.31] ← 送信元の IP を固定
```

| 入力メッセージ | 内容 |
|---|---|
| `<ch> <val> [<ch> <val> ...]` / `ch <start> v1 v2 ...` | naka.dmxusbpro と同じ。最初の値で送信が始まる |
| `blackout 1` / `blackout 0` | 一時的に全チャンネル 0 / 元に戻す |
| `bang` | いまのフレームをすぐ送る |
| `start` / `stop` | 送信を始める / 止める。`stop` のあとは値を変えても送らず、`start` で再開。sACN では止めるときに終了の印（Stream Terminated）を送る |
| `poll [<アドレス>]` | ArtPoll でノードを探す。左アウトレットに `node <ip> <短い名前> <長い名前> <出力のユニバース...>` |
| `getifaces` | このコンピュータのネットワークの一覧。左アウトレットに `iface <名前> <ip> <netmask> <broadcast>` |
| `print` | 設定と状態を Max コンソールへ |

| 属性 | 既定値 | 内容 |
|---|---|---|
| `@protocol` | `artnet` | `artnet` または `sacn` |
| `@universe` | 0（sACN は 1） | Art-Net はポートアドレス 0–32767（Net×256 + Sub-Net×16 + Universe）。sACN は 1–63999 |
| `@host` | 空 | 送り先の IP。空なら Art-Net はブロードキャスト、sACN はマルチキャスト（239.255.x.x） |
| `@bind` | 空 | 送信に使う自分の IP。Wi-Fi と有線の両方があるときに指定する |
| `@channels` | 512 | 送るチャンネル数（Art-Net は偶数に切り上げ） |
| `@refresh` | 40 | 最大送信レート（1–44 fps） |
| `@keepalive` | 1000 | 値が変わらなくても、この間隔（ms）で同じフレームを送り直す。0 で止める |
| `@priority` | 100 | sACN の優先度（0–200） |
| `@sourcename` | `naka.artnet (Max)` | sACN の送信元の名前 |

右アウトレットは送信中なら 1、止めているか送信に失敗しているなら 0。

**1 つの送り手しか受け付けない受信機について**: miseru 用の自作の箱は、最初に来た送り手の IP に「ロック」し、フレームが 5 秒途切れるとロックを外して全消灯します（[仕様 v0](https://github.com/akiranakayasu-eng/claude-shared-docs/blob/main/handoffs/2026-10-25_lighting-system__to__air-kicad_%E8%87%AA%E4%BD%9CODE%E3%81%AEWebSocket%E4%BB%95%E6%A7%98v0.md)）。naka.artnet は既定で 1 秒ごとに送り直すので、パッチを開いている間はロックを持ち続けます。ほかの人に照明を渡すときは `stop` を送ってください。送信元の IP が変わると別の送り手と見なされるので、`@bind` で固定しておくと確実です。ENTTEC ODE Mk2 は複数の送り手を受けたときの扱いが本体の設定（マージなど）次第なので、スタジオの iPad（Luminair）が送っている間は Max から同じユニバースに送らないでください。

## ビルド (Build)

CMake 3.25 以上。`max-sdk-base` は configure 時に自動取得します。

```sh
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release      # プロトコル（USB Pro / Art-Net / sACN）の単体テスト
```

## naka.miseru（miseru の色→DMX 変換）

[miseru](https://github.com/akiranakayasu-eng/miseru-lighting) と同じライト定義・同じ計算で、「明るさ・色の濃さ・色相・白のあたたかさ」の 4 つの数字から DMX の値を作ります。RGBW でも CCT 付きでも、同じ 4 つの数字で動かせます。変換するだけなので、出力は `naka.artnet`（ODE や自作の箱）か `naka.dmxusbpro` につないでください。

```
[studio(     ← TMU の撮影スタジオのパッチ（arri1-3、diva1-4）
[look all 1 0 0 5600(   ← 全部、明るさ 1、白、5600K
[naka.miseru]
[naka.artnet 10.7.157.222 0]
```

| メッセージ | 意味 |
|---|---|
| `patch <name> <type> <address> [<group>]` | ライトを足す。type は `dimmer` `rgb` `rgbw` `dim-rgb` `dim-rgbw` `arri-l7c` `diva-lite` |
| `studio` / `clear` | スタジオのパッチを読む / 全部消す |
| `look <target> <intensity> <sat> <hue> <cct>` | 見た目をまとめて指定。target はライト名・グループ名・`all`。intensity と sat は 0〜1、hue は 0〜360、cct はケルビン |
| `intensity` `sat` `hue` `cct` `<target> <value>` | 1 つだけ変える |
| `master <0..1>` / `blackout <0\|1>` | 全体の明るさ / 全消灯（見た目は保つ） |
| `allon` / `reset` | 消えているライトを点ける / 全部最初の状態へ |
| `bang` / `types` / `print` | 出し直す / 種類を表示 / パッチを表示 |

出力は `ch 1 <値...>`（最後のパッチ済みチャンネルまで）で、`naka.artnet` と `naka.dmxusbpro` にそのまま渡せます。変換の結果は miseru（TypeScript 版）と同じ値になることをテストで確認しています。Diva-Lite のチャンネル配置は miseru と同様に仮の定義です。

成果物は `externals/naka.dmxusbpro.mxo`、`externals/naka.artnet.mxo`、`externals/naka.miseru.mxo`（macOS universal）/ `.mxe64`（Windows）に出力されます。help パッチは `help/`、リファレンスは `docs/refpages/` にあります。このフォルダを Max の検索パスに入れるか、リポジトリごと `~/Documents/Max 9/Packages/` に置いてください。

GitHub Actions で macOS / Windows のビルドとテストを自動実行します（`.github/workflows/build.yml`）。

## ロードマップ

- [ ] 実機での送信確認（macOS Apple Silicon / Windows）
- [ ] ホットプラグ時の自動再接続
- [ ] 複数ウィジェットのシリアル番号指定オープン

## License

MIT — see [LICENSE](LICENSE). ENTTEC および DMX USB Pro は ENTTEC Pty Ltd の商標です。本プロジェクトは ENTTEC とは無関係です。
