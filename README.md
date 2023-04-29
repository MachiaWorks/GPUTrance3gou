# GPUTrance3gou

## depend
	glext.h and khrplatform.h
  Download from under url
	https://registry.khronos.org/OpenGL/index_gl.php

	and add your project.
  
  you may fix source because header files need to call.

# 日本語

## 必要な環境

VisualStudio 2019
フリー版でOK

## 必要なファイル

「glext.h」「khrplatform.h」が必要なので、
以下のサイトより落としてきてプロジェクトもしくはOpenGLのライブラリがあるフォルダに加えてください。
https://registry.khronos.org/OpenGL/index_gl.php
これはOpenGLの拡張機能を利用するためのヘッダファイルになります。
つまり拡張機能使って成立するシンセってことです。

うまく取り込めない場合はglext.hの中で「khrplatform.h」を呼び出してる部分があるので、
それをプロジェクト内のものを呼び出すように変更すれば動くはずです。

## コンパイルに必要な情報

* crinklerの利用が必須
https://github.com/runestubbe/Crinkler

あとはプロジェクトソリューションの中に書き込んでるけど、
思い出したら追記していきます。

