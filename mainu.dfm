object Form1: TForm1
  Left = 520
  Top = 106
  Width = 412
  Height = 418
  Caption = 'MDIO Parser'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object RichEdit1: TRichEdit
    Left = 0
    Top = 8
    Width = 401
    Height = 313
    TabOrder = 0
  end
  object pb: TProgressBar
    Left = 0
    Top = 328
    Width = 401
    Height = 16
    Min = 0
    Max = 10000000
    Position = 5000000
    TabOrder = 1
  end
  object Button1: TButton
    Left = 56
    Top = 360
    Width = 267
    Height = 25
    Caption = 'Save'
    TabOrder = 2
    OnClick = Button1Click
  end
  object Timer1: TTimer
    Interval = 10
    OnTimer = Timer1Timer
    Left = 248
    Top = 24
  end
end
