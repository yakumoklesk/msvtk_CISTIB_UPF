#include <gtest/gtest.h>

#include "BuildConfig.h"

#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include "vtkMultipleDataReader.h"
#include "vtkMultipleStructuredPointsReader.h"


#include <string>
using namespace std;

class TestMultipleStructuredPointsReader : public ::testing::Test
{
public:
    TestMultipleStructuredPointsReader()
    : m_MultipleStructuredPointsReaderSP( 0 )
    , m_FileNamesSP( 0 )
    {

    }
protected:
    virtual void SetUp()
    {
        string resFolder = GetResouceFolderPath();
        string heartModelMRI_VOLUME = resFolder + "HeartModel/MRI";

        m_MultipleStructuredPointsReaderSP = vtkSmartPointer<vtkMultipleStructuredPointsReader>::New();

        ASSERT_TRUE( m_MultipleStructuredPointsReaderSP.GetPointer() != 0 );

        m_MultipleStructuredPointsReaderSP->SetDirectoryName( heartModelMRI_VOLUME.c_str() );
        m_MultipleStructuredPointsReaderSP->SetWildCard( "*.vtk" );

        m_FileNamesSP.TakeReference( m_MultipleStructuredPointsReaderSP->GetFileNames() );
    }

    // virtual void TearDown() {}
    string GetResouceFolderPath()
    {
#if defined( BUILD_LOCATION )
#if( BUILD_LOCATION == BUILD_FROM_WORK )
        string resFolder = "F:/dev/prj/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
#elif( BUILD_LOCATION == BUILD_FROM_HOME )
        string resFolder = "C:/dev/prj/job/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
#endif
#else
        string resFolder = "H:/dev/prj/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
#endif  // #ifdef( BUILD_LOCATION )
        return resFolder;
    }

    vtkSmartPointer<vtkMultipleStructuredPointsReader> m_MultipleStructuredPointsReaderSP;
    vtkSmartPointer<vtkStringArray> m_FileNamesSP;
};

TEST_F( TestMultipleStructuredPointsReader, TestGetFileNames )
{
    EXPECT_EQ( 34, m_FileNamesSP->GetNumberOfTuples() );
}

TEST_F( TestMultipleStructuredPointsReader, TestGetFileNamesSeriesType )
{
    EXPECT_EQ( VTK_STRUCTURED_POINTS, m_MultipleStructuredPointsReaderSP->GetSeriesDataObjectType( m_FileNamesSP ) );

    /*
        case VTK_POLY_DATA:
        case VTK_STRUCTURED_POINTS:
        case VTK_STRUCTURED_GRID:
        case VTK_RECTILINEAR_GRID:
        case VTK_UNSTRUCTURED_GRID:
    */
}

