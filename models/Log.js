const mongoose = require('mongoose');

const logSchema = new mongoose.Schema({
  timestamp: { type: String, required: true },
  level: { type: String, required: true },
  message: { type: String, required: true }
});

module.exports = mongoose.model('Log', logSchema);
