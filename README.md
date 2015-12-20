# singenbd
シンプルな4opFMシンセサイザ。
いかにもシンセサイザ、という打楽器音をランダマイズでさくっと作るためのソフト。

## Oscillator
FMの構成は下図の通り。

```
+-+
| |
3-+
|
2-+
| |
1 sub
+-+
|
FX
|
output
```
  
### Pitch
整数部がmidiノートナンバーに対応している。A4 (440Hz) = 69。小数部はcent値。

### OscType
Sin, Saw, Squareが選べる。Saw, SquareはEnvelope/Shapeによって波形を変形できる。

### Phase
音の開始時の位相。アタックの質感を微妙に調整できる。

### Mod
FM変調指数(FM Index)。

### Mute
オシレータをミュート。


## Envelope
ディケイのみのシンプルなエンベロープ。
パラメータあたり2つのエンベロープが利用でき、それぞれe1, e2とすると出力は以下。

```
output = clamp(e1(t) + e2(t), 0.0, 1.0); // tは時間
```

このエンベロープによって音の出だしをより細かく調整することができる。
設定に迷ったら、D1.Timeを短め、D2.Timeを長めにして、他は適当にランダマイズでそれっぽくなる。

### Dn.Gain
エンベロープの最大値。

### Dn.Time
減衰時間。

### Dn.Tension
減衰カーブの特性。

### Dn.Type
減衰カーブの種類。


## FX
音に癖をつけるエフェクト。

### Delay.Time
ディレイ時間。

### Delay.Decay
ディレイのフィードバックの大きさ。

### Clip.Gain
クリッパに入力する音の大きさ。

### Clip.Decay
クリッパのかかり方。Clip.Gainの値をgain, Clip.Decayの値をdecayとすると出力は以下。

```
output = clip((2.0 - gain * e_c(t)) * input); // tは時間

ここで

e_c(t)
{
  dd = decay / d_max; // [0.0, 1.0] の範囲に正規化。d_max は Clip.Decay の最大値
  return min(1.0, t / (duration * dd)); // duration は Misc/Durationの値
}
```


## Misc.

### Duration
音の長さ。表示されている値の単位は秒。音の長さは最低でも200サンプルに設定される。

### SampleRate
サンプリング周波数。

### SuperSampling
単純な平均によるアンチエイリアシング。値を大きくするとレンダリングが非常に重くなるため注意。

### DC Block
直流ブロッキングフィルタ。

### Declick
音の開始・終了部のクリックノイズ除去。200サンプル長のフェードイン・フェードアウト。

## File

### Prefix
保存されるファイル名の前置詞。ファイル名は以下のように決まる。

```
[Prefix].ini
[Prefix]_[currentDateTime].wav
```

### InstaSaveWav
右下のSaveボタンを押す一手間を省くための設定。
チェックが入っている場合、右下のRandom, Renderボタン押下時に、自動的にwavを書き出す。
wavはrenderディレクトリに保存される。

### Save .ini
iniファイルに現在の設定を保存する。

### Load .ini
iniファイルから設定を読み込む。

### オートセーブ・ロードについて
SingenBDはオートセーブ・ロードのため、temp.iniというファイルを作成する。
temp.iniはSingenBDの起動時に読み込まれ、終了時に書き込まれる。
temp.iniが存在していない場合は、デフォルトの設定が書かれたtemp.iniがSingenBDの終了時、
実行ファイルと同じディレクトリに作成される。


## 右下

### Random
名前にチェックが入っているパラメータについてランダマイズを行う。
Oscillator, Envelopeのランダマイズ設定はすべてのOscillatorで共有される。

### Render
レンダリングを行う。

### Play
レンダリングされた音を再生する。

### Save
レンダリングされた音をrenderディレクトリに保存する。

### 波形
ホイールでズームイン・アウト、ドラッグで左右にスクロールできる。
左下の表示は[左端のサンプル番号]/[総サンプル数]。

