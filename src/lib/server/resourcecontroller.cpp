#include "resourcecontroller.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>
#include <RestLink/httputils.h>

#include <RestLink/debug.h>

namespace RestLink {

/*!
 * \class AbstractController
 * \brief Defines the base interface for all server-side controllers.
 *
 * This class provides a contract for handling incoming server requests.
 * Subclasses must implement the `endpoint()` and `processRequest()` methods.
 * A generic data source can be associated with the controller using `setDataSource()`.
 */

AbstractController::AbstractController()
    : m_dataSource(nullptr)
{
}

AbstractController::~AbstractController()
{
}

/*!
 * \fn virtual QString AbstractController::endpoint() const
 * \brief Returns the base endpoint this controller is responsible for.
 */

/*!
 * \brief Determines whether the controller can process the given request.
 */
bool AbstractController::canProcessRequest(const ServerRequest &request) const
{
    return request.endpoint() == endpoint();
}

/*!
 * \fn virtual void AbstractController::processRequest(const ServerRequest &request, ServerResponse *response)
 * \brief Processes the incoming request and writes the response.
 */

/*!
 * \brief Returns the data source associated with this controller.
 */
void *AbstractController::dataSource() const
{
    return m_dataSource;
}

/*!
 * \brief Sets the data source for the controller.
 *
 * \param source A generic pointer to a data source, typically used by the controller
 *               to retrieve or manipulate data.
 */
void AbstractController::setDataSource(void *source)
{
    m_dataSource = source;
}

/*!
 * \class AbstractResourceController
 * \brief Provides a base interface for RESTful resource controllers.
 *
 * This abstract class defines standard CRUD operations (`index`, `show`, `update`, `store`, `destroy`)
 * to be implemented by subclasses for handling requests in a RESTful manner.
 * It also overrides request processing logic to dispatch requests to the appropriate method.
 */

/*!
 * \fn AbstractResourceController::~AbstractResourceController()
 * \brief Virtual destructor for cleanup in derived classes.
 */

/*!
 * \fn void AbstractResourceController::index(const ServerRequest &request, ServerResponse *response)
 * \brief Handles the listing of resources.
 *
 * This method should be implemented to respond to a GET request on a collection route.
 */

/*!
 * \fn void AbstractResourceController::show(const ServerRequest &request, ServerResponse *response)
 * \brief Handles showing a single resource.
 *
 * This method should be implemented to respond to a GET request on a single item route.
 */

/*!
 * \fn void AbstractResourceController::update(const ServerRequest &request, ServerResponse *response)
 * \brief Handles updating a resource.
 *
 * This method should be implemented to respond to a PUT or PATCH request on a single item route.
 */

/*!
 * \fn void AbstractResourceController::store(const ServerRequest &request, ServerResponse *response)
 * \brief Handles creating a new resource.
 *
 * This method should be implemented to respond to a POST request on a collection route.
 */

/*!
 * \fn void AbstractResourceController::destroy(const ServerRequest &request, ServerResponse *response)
 * \brief Handles deleting a resource.
 *
 * This method should be implemented to respond to a DELETE request on a single item route.
 */

/*!
 * \brief Checks whether the controller can process the given request.
 *
 * Typically used to determine if the controller supports the request method and route.
 */
bool AbstractResourceController::canProcessRequest(const ServerRequest &request) const
{
    const QString routeEndpoint = this->endpoint();
    QString requestEndpoint = request.endpoint();

    if (requestEndpoint == routeEndpoint)
        return true;

    requestEndpoint.remove(this->endpoint() + '/');
    return requestEndpoint.count('/') == 0;
}

/*!
 * \brief Processes the incoming request and dispatches it to the appropriate handler method.
 *
 * Dispatch logic typically maps request methods to controller actions like index, show, update, etc.
 */
void AbstractResourceController::processRequest(const ServerRequest &request, ServerResponse *response)
{
    RequestHandler::Method method = request.method();

    if (method == RequestHandler::GetMethod) {
        if (request.identifier().isEmpty())
            index(request, response);
        else
            show(request, response);
        return;
    }

    if (method == RequestHandler::PostMethod) {
        store(request, response);
        return;
    }

    if (method == RequestHandler::PutMethod) {
        update(request, response);
        return;
    }

    if (method == RequestHandler::DeleteMethod) {
        destroy(request, response);
        return;
    }

    restlinkDebug() << "Resource controller for endpoint " << endpoint() << " can't handle request: "
                    << HttpUtils::verbString(method) << ' ' << request.endpoint();
}

} // namespace RestLink
