// Import the Express module
import express from "express";

// Import the CORS module
import cors from "cors";

import indexRoutes from "./routes/index.js";
import authRoutes from "./routes/auth.js";

// Create an Express application
const app = express();

const setXContentTypeOptions = (req, res, next) => {
  res.set("x-content-type-options", "nosniff");
  next();
};


const setXFrameOptions = (req, res, next) => {
  res.set("x-frame-options", "deny");
  next();
};

const setContentSecurityPolicy = (req, res, next) => {
  res.set("content-security-policy", "default-src 'none'");
  next();
};

app.use(cors()); 


app.use(setXContentTypeOptions);
app.use(setXFrameOptions);
app.use(setContentSecurityPolicy);



// Use the PORT environment variable or 3000
const PORT = process.env.PORT || 3000;

// Use the CORS module

app.use(express.urlencoded({ extended: false }));
app.use(express.json()); 
app.use("/api/plants", indexRoutes);
app.use("/api/auth", authRoutes);

// Start the server on port 3000
app.listen(PORT, () => {
  console.log(`Server is listening on port ${PORT}.`);
});

// Export the Express application. Other modules may use it. For example, API testing
export default app;