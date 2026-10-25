# Multi-stage build using debian:bookworm-slim for BOTH builder and runtime
FROM debian:bookworm-slim AS builder

# Install GCC C++ compiler on debian:bookworm
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    make \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY httplib.h server.cpp ./

# Compile with static libstdc++ to guarantee zero GLIBCXX mismatches
RUN g++ -std=c++17 -O2 -static-libgcc -static-libstdc++ server.cpp -o server -lpthread

# Final minimal runtime image with exact same debian:bookworm-slim base
FROM debian:bookworm-slim
WORKDIR /app

# Copy compiled C++ binary and frontend assets
COPY --from=builder /app/server .
COPY index.html style.css app.js ./

# Cloud platforms (Render, Railway, Fly.io) inject PORT dynamically
ENV PORT=8080
EXPOSE 8080

CMD ["./server"]
