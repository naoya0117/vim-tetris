# ビルドステージ
FROM ubuntu:oracular AS builder

COPY ./src /app
WORKDIR /app

# 必要なパッケージをインストール
RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc make libncurses-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && \
    # tvimのコンパイル
    make -f /app/makefile

# ランタイムステージ
FROM ubuntu:oracular

# 必要なパッケージをインストール
RUN apt-get update && apt-get install -y --no-install-recommends \
        language-pack-ja \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && \
    # 日本語ロケールを生成
    locale-gen ja_JP.UTF-8

WORKDIR /app

# ビルド成果物をコピー
COPY --chmod=755 --from=builder /app/tvim /usr/local/bin/tvim
COPY ./src/help.txt /app/help.txt

# 環境変数を設定
ENV LANG=ja_JP.UTF-8
