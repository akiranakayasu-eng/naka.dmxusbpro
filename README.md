# naka.dmxusbpro for Max

ENTTEC DMX USB Pro を Cycling '74 Max から直接動かすためのオープンソース外部オブジェクトです。
An open-source Max external for the ENTTEC DMX USB Pro (macOS Apple Silicon / Intel, Windows).

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

## インストール (Install)

1. [Releases](https://github.com/akiranakayasu-eng/naka.dmxusbpro/releases) から `naka.dmxusbpro-<版>.zip` をダウンロードして展開する（開発中の最新版は Actions の各実行の `naka.dmxusbpro-package` から取れます）。
2. 出てきた `naka.dmxusbpro` フォルダを Max の Packages フォルダに入れる。
   - macOS: `~/Documents/Max 9/Packages/`
   - Windows: `Documents\Max 9\Packages\`
3. Max を再起動し、パッチに `naka.dmxusbpro` と打つ。help はオブジェクトを右クリック →「Open naka.dmxusbpro Help」。

macOS で「開発元を検証できない」と出たときは、ターミナルで次を一度実行してください（署名はアドホック署名のみで、Apple の公証はまだ受けていません）。

```sh
xattr -dr com.apple.quarantine ~/Documents/Max\ 9/Packages/naka.dmxusbpro
```

パッケージの中身: `externals/`（macOS universal の `.mxo` と Windows x64 の `.mxe64`）、`help/`、`docs/refpages/`、`package-info.json`。

## ビルド (Build)

CMake 3.25 以上。`max-sdk-base` は configure 時に自動取得します。

```sh
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release      # プロトコルの単体テスト
```

成果物は `externals/naka.dmxusbpro.mxo`（macOS universal）/ `externals/naka.dmxusbpro.mxe64`（Windows）に出力されます。help パッチは `help/naka.dmxusbpro.maxhelp`、リファレンスは `docs/refpages/` にあります。このフォルダを Max の検索パスに入れるか、リポジトリごと `~/Documents/Max 9/Packages/` に置いてください。

GitHub Actions で macOS / Windows のビルドとテストを自動実行し、両方の外部オブジェクトをまとめた Max Package の zip を作ります（`.github/workflows/build.yml`）。手元でまとめるときは `scripts/make_package.sh <macの externals> <Windowsの externals> <出力先>` を使います。

### リリース手順

1. `package-info.json` の `version` と `naka.dmxusbpro.c` の `NAKA_DMXUSBPRO_VERSION` を同じ版に上げる（ずれていると CI が止まります）。
2. `git tag v<版> && git push origin v<版>` すると、CI が zip を添付したリリースの下書きを作ります。
3. GitHub の Releases で下書きを確認して公開する。

## ロードマップ

- [ ] 実機での送信確認（macOS Apple Silicon / Windows）
- [ ] ホットプラグ時の自動再接続
- [ ] 複数ウィジェットのシリアル番号指定オープン

## License

MIT — see [LICENSE](LICENSE). ENTTEC および DMX USB Pro は ENTTEC Pty Ltd の商標です。本プロジェクトは ENTTEC とは無関係です。
