FROM node:18-bullseye

# Install g++ to compile the C++ code
RUN apt-get update && apt-get install -y g++

# Set working directory
WORKDIR /app

# Copy package files and install Node dependencies
COPY package*.json ./
RUN npm install

# Copy all source code
COPY . .

# Compile the C++ code into a Linux executable
RUN g++ src/main.cpp src/parser.cpp src/detector.cpp -o monitor

# Expose the port
EXPOSE 3000

# Start the server
CMD ["npm", "start"]
