
#include "Professor/SimplexInterpolation.h"
#include "Professor/ParamPoints.h"

#include <numeric>
#include <algorithm>


// CGAL stuff

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
// #include <CGAL/Triangle_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2< double, K> Vb;
typedef CGAL::Triangulation_data_structure_2<Vb> Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds> Triangulation;
//typedef CGAL::Triangle_2<K> Triangle;
typedef Triangulation::Point Point;
typedef Triangulation::Face Face;
typedef Triangulation::Vertex_handle Vertex_handle;
typedef Triangulation::Face_handle Face_handle;
typedef Triangulation::Face_iterator Face_iterator;
typedef Triangulation::Face_circulator Face_circulator;



namespace Professor {

  // ctor, store values together with parameters so they get sorted together
  SimplexInterpolation::SimplexInterpolation( const ParamPoints& pts, 
					      const std::vector<double>& ptvals ) {
    const std::vector< std::vector<double> > & ParameterPoints= pts.points();
    for( size_t i= 0; i < ParameterPoints.size(); i++ ) {
      std::vector<double> point= ParameterPoints[i];
      point.push_back( ptvals[i] );
      points.push_back( point );
    }
  }
  
  std::string SimplexInterpolation::toString( const std::string& str ) const {
    return "not yet";
  }
  
  void SimplexInterpolation::fromString( const std::string& str ) {
    std::cout << "not yet" << std::endl;
  }
  
  // Add squares of vector<double> arrays for euclidian norm
  class add_square {
  public:
    double operator() ( double sum, double nextElement ) const {
      return sum + nextElement*nextElement;
    }
  } addsq;
  
  // Compare parameter points distance w.r.t. to point by euclidian distance
  class EuclidianDistanceComparator {
  public:
    EuclidianDistanceComparator( const std::vector<double>& p ) : parameters( p ) {}
    bool operator() ( const std::vector<double>& p1,
		      const std::vector<double>& p2 ) const {
      std::vector<double> point1( p1 );
      std::vector<double> point2( p2 );
      std::transform( parameters.begin(), parameters.end(), point1.begin(), 
		      point1.begin(), std::minus<double>() );
      std::transform( parameters.begin(), parameters.end(), point2.begin(), 
		      point2.begin(), std::minus<double>() );
      double d1= std::accumulate( point1.begin(), point1.end()-1, 0.0, addsq );
      double d2= std::accumulate( point2.begin(), point2.end()-1, 0.0, addsq );
      return d1 < d2;
    }
  private:
    std::vector<double> parameters;
  };
    
  double SimplexInterpolation::value( const std::vector<double>& parameters ) const {
    EuclidianDistanceComparator distance( parameters );

    // std::cout << "Parameters:";
    // for( auto p : parameters ) std::cout << " " << p;
    // std::cout << std::endl;

    // std::cout << "Points:";
    // for( std::vector<double> point : points )
    //   for( auto p : point ) std::cout << " " << p;
    // std::cout << std::endl;

    std::vector< std::vector<double> > sortedPoints( points );
    std::sort( sortedPoints.begin(), sortedPoints.end(), distance );

    // std::cout << "Sorted Points:";
    // for( std::vector<double> point : sortedPoints )
    //   for( auto p : point ) std::cout << " " << p;
    // std::cout << std::endl;

    size_t ndim= parameters.size();
    double result= 0.0;
    if( ndim == 1 ) {
      std::vector<double> closest= sortedPoints[0];
      std::vector<double> nextclosest= sortedPoints[1];
      double a= closest[0];
      double b= nextclosest[0];
      double p= parameters[0];
      double valuea= closest[1];
      double valueb= nextclosest[1];
      double deltaa= p - a;
      double deltab= b - p;
      double delta= b - a;
      if( fabs(deltaa)+fabs(deltab)-fabs(delta) > 1.0e-6 ) {
	std::cout << "1d Point is outside" << std::endl;
      }
      result= ( valuea*deltab + valueb*deltaa ) / delta;
    }
    else if( ndim == 2 ) {
      // Do Delaunay triangulation, find matching triangle, interpolate
      std::vector< std::pair<Point,double> > cgalPoints;
      for( std::vector<double> vp : points ) {
	cgalPoints.push_back( std::make_pair( Point( vp[0], vp[1] ), vp[2] ) );
      }
      Triangulation T;
      T.insert( cgalPoints.begin(), cgalPoints.end() );
      Point parameterPoint( parameters[0], parameters[1] );
      Face_handle fh= T.locate( parameterPoint );
      std::cout << "Point: (" << parameterPoint << ")";
      if( T.is_infinite( fh ) ) {
	// neighbor search can wrap around and return wrong triangle
	// std::cout << " is outside, use closest triangle:";
	// Face_handle fhn;
	// for( size_t i= 0; i < 3; i++ ) {
	//   fhn= fh->neighbor( i );
	//   if( not T.is_infinite( fhn ) ) break;
	// }
	// fh= fhn;
	// search with nearest_vertex instead
	Vertex_handle vh= T.nearest_vertex( parameterPoint );
	std::cout << vh->point();
	Face_circulator fc= T.incident_faces( vh );
	Face_circulator done( fc );
	if( fc != 0 ) {
	  do if( not T.is_infinite( fc ) ) break;
	  while( ++fc != done );
	}
	fh= fc;	
      }
      std::vector< std::vector<double> > vvp;
      for( size_t i= 0; i < 3 ; i++ ) {
	Vertex_handle vh= fh->vertex( i );
	std::vector<double> vp= { vh->point().x(), vh->point().y(), vh->info() };
	vvp.push_back( vp );
	std::cout << " (" << vp[0] << " " <<  vp[1] << ") " << vp[2];
      }
      result= barycentricInterpolation_2d( parameters, vvp[0], vvp[1], vvp[2] );   

      std::cout << " interpolation: " << result << std::endl;

    }
    else {
      std::cout << "not yet" << std::endl;
    }
    return result;
  }

  double SimplexInterpolation::barycentricInterpolation_2d( std::vector<double> parameters,
							    std::vector<double> p1,
							    std::vector<double> p2,
							    std::vector<double> p3 ) const {
    double x= parameters[0];
    double y= parameters[1];
    double x1= p1[0];
    double y1= p1[1];
    double value1= p1[2];
    double x2= p2[0];
    double y2= p2[1];
    double value2= p2[2];
    double x3= p3[0];
    double y3= p3[1];
    double value3= p3[2];
    double norm= (y2-y3)*(x1-x3)+(x3-x2)*(y1-y3);
    double lambda1= ( (y2-y3)*(x-x3)+(x3-x2)*(y-y3) )/norm;
    double lambda2= ( (y3-y1)*(x-x3)+(x1-x3)*(y-y3) )/norm;
    double lambda3= 1.0-lambda1-lambda2;
    if( lambda1 >= 1.0 or lambda2 >= 1.0 or lambda3 >= 1.0 ) {
      std::cout << "bci_2: Point (" << x << " " << y << ") is outside" << std::endl;
    }
    return value1*lambda1+value2*lambda2+value3*lambda3;
  }

  double SimplexInterpolation::derivative( const std::vector<double>& parameters ) const {
    return -1;
  }
  
  std::vector<double> 
  SimplexInterpolation::gradient( const std::vector<double>& parameters ) const {
    return std::vector<double>();
  }
  
  
}

