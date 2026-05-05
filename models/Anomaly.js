const mongoose = require('mongoose');

const anomalySchema = new mongoose.Schema({
  type: { type: String, required: true },
  message: { type: String, required: true },
  timestamp: { type: String, required: true }
});

module.exports = mongoose.model('Anomaly', anomalySchema);
