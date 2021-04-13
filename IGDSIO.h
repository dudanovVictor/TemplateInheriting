#ifndef _I_GDS_READER_H_
#define _I_GDS_READER_H_

#include "../Core/ContourUtils.h"

#include <QPoint>
#include <QString>
#include <QVector>

/// GDS reader interface
class IGDSReader
{
public:
    virtual ~IGDSReader(){}

    /// Reads GDS file by filename
    virtual bool Read(QString filename) = 0;
    /// Getters for unit size
    virtual double GetPhysicalUnit() const = 0;
    virtual double GetUserUnit() const = 0;
    virtual void GetUsedLayers(std::vector<int>& result) const = 0;
    virtual QPointF GetLayer(int layer, emt::FPolygons& contours) const = 0;

    /// Extracts contours
    /// @param[in, out] o_lines - container for contour's lines
    /// \return normalization offset ([-x, xx] - > [0, xx+x])
    virtual QPointF GetContours(emt::FPolygons& lines, emt::FPolygons* mask = NULL) const = 0;

protected:
    IGDSReader(){}
};

/// GDS writer interface
class IGDSWriter
{
public:
    virtual ~IGDSWriter(){}

    /// Creates GDS file with given filename and unit sizes
    virtual bool Create(QString filename, double physicalUnit, double userUnit) = 0;
    /// Adds a simple lines to chosen layer to GDS file
    /// @param[in] layerNumber - the number of layer
    /// @param[in] points - array of contour's points
    /// @param[in] polygonMode - true if points are closed (polygon), false otherwise
    virtual void AddToLayer(short layerNumber, const emt::FPolygon& points, bool polygonMode) = 0;
    /// Adds a complex shapes (with holes) to chosen layer to GDS file
    /// @param[in] layerNumber - the number of layer
    /// @param[in] shapes - array of aray of contour's points
    /// @param[in] polygonMode - true if points are closed (polygon), false otherwise
	void AddShapesToLayer(short layerNumber, const emt::FPolygons& shapes, bool polygonMode)
	{
		emt::FPolygons figureWithHoles;
		ContourUtils::EmbedHoles(shapes, figureWithHoles);
		for (size_t i = 0; i < figureWithHoles.size(); ++i)
			AddToLayer(layerNumber, figureWithHoles[i], polygonMode);
	}

protected:
    IGDSWriter() { }
};

#endif
