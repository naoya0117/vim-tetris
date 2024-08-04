# ビルドステージ
FROM ubuntu:oracular-20240617 AS builder

COPY ./src /app
WORKDIR /app

# 必要なパッケージをインストール
RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc=4:14.1.0-2ubuntu1 \
        make=4.3-4.1build2 \
        libncurses-dev=6.5-2 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && \
    # tvimのコンパイル
    make -f /app/makefile

# ランタイムステージ
FROM ubuntu:oracular-20240617

# 必要なパッケージをインストール
RUN apt-get update && apt-get install -y --no-install-recommends \
        language-pack-ja=1:24.04+20240419 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && \
    # 日本語ロケールを生成
    locale-gen ja_JP.UTF-8

# ビルド成果物をコピー
COPY --chmod=755 --from=builder /app/tvim /usr/local/bin/tvim

# 環境変数を設定
ENV LANG=ja_JP.UTF-8
