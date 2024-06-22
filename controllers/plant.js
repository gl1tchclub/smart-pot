import { PrismaClient, Prisma } from "@prisma/client";
const prisma = new PrismaClient();

const createPlant = async (req, res) => {
  try {
    // Validate the content-type request header. It ensures that the request body is in JSON format
    const contentType = req.headers["content-type"];
    if (!contentType || contentType !== "application/json") {
      return res.status(400).json({
        msg: "Invalid Content-Type. Expected application/json.",
      });
    }
  } catch (error) {
    if (err instanceof Prisma.PrismaClientKnownRequestError) {
      {
        if (err.code === "P2002") {
          return res.status(400).json({
            msg: "Plant with the same name already exists",
          });
        }
      }
    }
  }
};

const getPlants = async (req, res) => {
  try {
    const institutions = await prisma.institution.findMany();

    // Check if there are no institutions
    if (!institutions) {
      return res.status(404).json({ msg: "No institutions found" });
    }

    return res.status(200).json({
      data: institutions,
    });
  } catch (err) {
    return res.status(500).json({
      msg: err.message,
    });
  }
};

const getPlant = async (req, res) => {
  try {
    const institution = await prisma.institution.findUnique({
      where: { id: Number(req.params.id) },
    });

    // Check if there is no institution
    if (!institution) {
      return res
        .status(404)
        .json({ msg: `No institution with the id: ${req.params.id} found` });
    }

    return res.status(200).json({
      data: institution,
    });
  } catch (err) {
    return res.status(500).json({
      msg: err.message,
    });
  }
};

export { createPlant, getPlants, getPlant };
