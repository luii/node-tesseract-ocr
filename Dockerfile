FROM node:lts-bookworm as builder

RUN apt-get install -y \
  python3 \
  build-essential \
  pkg-config \
  libtesseract-dev \
  libleptonica-dev \
  tesseract-ocr-eng \
  && rm -rf /var/lib/apt/lists/*

ENV PYTHON=/usr/bin/python3

WORKDIR /app

COPY package*.json ./

RUN npm ci && npm cache clean --force

COPY . .

# builds:
# - native Addon (Release)
# - CJS into dist/cjs/index.cjs
# - ESM into dist/esm/index.mjs
RUN npm run build:release \
  && cp build/Release/node-tesseract-ocr.node dist/

FROM scratch AS export

WORKDIR /

COPY --from=builder /app/dist ./
