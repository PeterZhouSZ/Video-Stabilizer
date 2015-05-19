#ifndef STABILIZER_HPP
#define STABILIZER_HPP


#include "opencv2/opencv.hpp"
#include "utilities.h"

/**
 * @brief Interface for video stabilization.
 */

class Stabilizer
{
public:
    enum class Warping {
        HOMOGRAPHY,         // arbitrary homography
        ROT_HOMOGRAPHY,     // homographies generated by camera rotations
        AFFINE,             // affine transformations
        RIGID,              // rigid transformations (translation + rotation)
        TRANSLATION,        // only translation
    };

    enum class Mode {
        DIRECT,             // directly compare current frame to reference
        TRACK_REF,          // always compare to previous frame
        WARP_BACK           // warp current frame back to reference and compare to reference
    };

public:
    Stabilizer (const cv::Mat& frame_0, Warping warping=Warping::HOMOGRAPHY, Mode mode=Mode::DIRECT, bool visualize=true)
        : warping_{warping}, mode_{mode}, visualize_{visualize} {
        H_ = cv::Mat::eye(3, 3, CV_64FC1);
        cv::cvtColor(frame_0, ref_frame_gray_, CV_BGR2GRAY);
    }

    /**
     * @brief Stabilize next frame. Calls Stabilizer::get_next_homography to get the homography.
     * @param next_frame        The next colored frame to stabilize.
     * @return                  The stabilized frame.
     */
    virtual cv::Mat stabilize_next(const cv::Mat& next_frame);

    /**
     * @brief Get the visualization for the last stabilization.
     * @return The visualization.
     */
    virtual cv::Mat visualization() const {
        return visualization_;
    }

    static cv::Mat find_homography(const cv::vector<cv::Point2f>& points0, const cv::vector<cv::Point2f>& points1, Warping mode=Warping::HOMOGRAPHY);

protected:
    virtual cv::Mat get_next_homography(const cv::Mat& next_frame) = 0;
    virtual void create_visualization() = 0;


protected:
    // Parameters
    Warping warping_ = Warping::HOMOGRAPHY; // warping group used for stabilization
    Mode mode_;                             // stabilization mode (see Mode description)
    bool visualize_ = true;                 // visualize stuff?

    // State
    cv::Mat H_;                 // the current homography

    cv::Mat ref_frame_gray_;    // current reference frame in gray
    cv::Mat frame;              // the current frame (warped if mode_==Mode::WARP_BACK)
    cv::Mat frame_gray_;        // the current frame in gray (warped if mode_==Mode::WARP_BACK)
    cv::Mat H_frame_gray_;      // delete?

    cv::Mat visualization_;     // visualization of last stabilization
};

#endif // STABILIZER_HPP
