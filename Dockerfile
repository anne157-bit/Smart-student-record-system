# Multi-stage build for ultra-lightweight C++ container
FROM gcc:13 AS builder

WORKDIR /app
COPY httplib.h server.cpp ./
RUN g++ -std=c++17 -O2 server.cpp -o server -lpthread

# Minimal runtime image
FROM debian:bookworm-slim
WORKDIR /app

# Copy compiled C++ binary and frontend assets
COPY --from=builder /app/server .
COPY index.html style.css app.js ./

# Cloud platforms (Render, Railway, Fly.io) inject PORT dynamically
ENV PORT=8080
EXPOSE 8080

CMD ["./server"]
