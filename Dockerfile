FROM node:22-bookworm as builder

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

COPY package.json package-lock.json ./

RUN npm ci --ignore-scripts && \
  npm cache clean --force

COPY src ./
COPY lib ./
COPY tsconfig.(base|cjs|esm).json ./
COPY CMakeLists.txt ./
COPY binding-options.js ./

RUN npm run build:release \
  && cp build/Release/node-tesseract-ocr.node dist/

FROM scratch AS export

WORKDIR /

COPY --from=builder /app/dist ./
